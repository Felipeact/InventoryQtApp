#include "ApiClient.h"
#include "Logging.h"

#include <nlohmann/json.hpp>
#include <cpr/timeout.h>

using json = nlohmann::json;

namespace
{
    // Network timeout for every request, in milliseconds.
    constexpr long kRequestTimeoutMs = 5000;
}

ApiClient::ApiClient(const std::string& baseUrl)
    : baseUrl(baseUrl)
{
}

void ApiClient::setAccessToken(const std::string& token)
{
    std::lock_guard<std::mutex> lock(tokenMutex);
    accessToken = token;
}

void ApiClient::setRefreshToken(const std::string& token)
{
    std::lock_guard<std::mutex> lock(tokenMutex);
    refreshToken = token;
}

std::string ApiClient::getAccessToken() const
{
    std::lock_guard<std::mutex> lock(tokenMutex);
    return accessToken;
}

std::string ApiClient::getRefreshToken() const
{
    std::lock_guard<std::mutex> lock(tokenMutex);
    return refreshToken;
}

void ApiClient::clearTokens()
{
    std::lock_guard<std::mutex> lock(tokenMutex);
    accessToken.clear();
    refreshToken.clear();
}

cpr::Header ApiClient::makeAuthHeader(const std::string& token)
{
    if (token.empty()) {
        return cpr::Header{};
    }

    return cpr::Header{
        {"Authorization", "Bearer " + token}
    };
}

cpr::Response ApiClient::get(const std::string& endpoint)
{
    std::string token = getAccessToken();

    auto res = cpr::Get(
        cpr::Url{ baseUrl + endpoint },
        makeAuthHeader(token),
        cpr::Timeout{ kRequestTimeoutMs }
    );

    inv::logDebug("GET " + endpoint + " -> " + std::to_string(res.status_code));

    if (res.status_code == 401 && refreshAccessToken(token)) {
        res = cpr::Get(
            cpr::Url{ baseUrl + endpoint },
            makeAuthHeader(getAccessToken()),
            cpr::Timeout{ kRequestTimeoutMs }
        );
    }

    return res;
}

cpr::Response ApiClient::post(const std::string& endpoint, const std::string& body)
{
    const bool isAuthEndpoint =
        (endpoint == "/auth/login" || endpoint == "/auth/refresh");

    std::string token = getAccessToken();

    cpr::Header headers{
        {"Content-Type", "application/json"}
    };

    if (!isAuthEndpoint && !token.empty()) {
        headers["Authorization"] = "Bearer " + token;
    }

    auto res = cpr::Post(
        cpr::Url{ baseUrl + endpoint },
        headers,
        cpr::Body{ body },
        cpr::Timeout{ kRequestTimeoutMs }
    );

    inv::logDebug("POST " + endpoint + " -> " + std::to_string(res.status_code));

    if (res.status_code == 401 && !isAuthEndpoint && refreshAccessToken(token)) {
        cpr::Header retryHeaders{
            {"Content-Type", "application/json"},
            {"Authorization", "Bearer " + getAccessToken()}
        };

        res = cpr::Post(
            cpr::Url{ baseUrl + endpoint },
            retryHeaders,
            cpr::Body{ body },
            cpr::Timeout{ kRequestTimeoutMs }
        );
    }

    return res;
}

cpr::Response ApiClient::put(const std::string& endpoint, const std::string& body)
{
    std::string token = getAccessToken();

    cpr::Header headers{
        {"Content-Type", "application/json"}
    };

    if (!token.empty()) {
        headers["Authorization"] = "Bearer " + token;
    }

    auto res = cpr::Put(
        cpr::Url{ baseUrl + endpoint },
        headers,
        cpr::Body{ body },
        cpr::Timeout{ kRequestTimeoutMs }
    );

    inv::logDebug("PUT " + endpoint + " -> " + std::to_string(res.status_code));

    if (res.status_code == 401 && refreshAccessToken(token)) {
        cpr::Header retryHeaders{
            {"Content-Type", "application/json"},
            {"Authorization", "Bearer " + getAccessToken()}
        };

        res = cpr::Put(
            cpr::Url{ baseUrl + endpoint },
            retryHeaders,
            cpr::Body{ body },
            cpr::Timeout{ kRequestTimeoutMs }
        );
    }

    return res;
}

cpr::Response ApiClient::patch(const std::string& endpoint, const std::string& body)
{
    std::string token = getAccessToken();

    cpr::Header headers{
        {"Content-Type", "application/json"}
    };

    if (!token.empty()) {
        headers["Authorization"] = "Bearer " + token;
    }

    auto res = cpr::Patch(
        cpr::Url{ baseUrl + endpoint },
        headers,
        cpr::Body{ body },
        cpr::Timeout{ kRequestTimeoutMs }
    );

    inv::logDebug("PATCH " + endpoint + " -> " + std::to_string(res.status_code));

    if (res.status_code == 401 && refreshAccessToken(token)) {
        cpr::Header retryHeaders{
            {"Content-Type", "application/json"},
            {"Authorization", "Bearer " + getAccessToken()}
        };

        res = cpr::Patch(
            cpr::Url{ baseUrl + endpoint },
            retryHeaders,
            cpr::Body{ body },
            cpr::Timeout{ kRequestTimeoutMs }
        );
    }

    return res;
}

cpr::Response ApiClient::del(const std::string& endpoint)
{
    std::string token = getAccessToken();

    auto res = cpr::Delete(
        cpr::Url{ baseUrl + endpoint },
        makeAuthHeader(token),
        cpr::Timeout{ kRequestTimeoutMs }
    );

    inv::logDebug("DELETE " + endpoint + " -> " + std::to_string(res.status_code));

    if (res.status_code == 401 && refreshAccessToken(token)) {
        res = cpr::Delete(
            cpr::Url{ baseUrl + endpoint },
            makeAuthHeader(getAccessToken()),
            cpr::Timeout{ kRequestTimeoutMs }
        );
    }

    return res;
}

bool ApiClient::validateToken(std::string& role, std::vector<std::string>& permissions)
{
    try {
        auto res = get("/auth/validate");

        if (res.status_code != 200) {
            return false;
        }

        auto data = json::parse(res.text);

        if (!data.contains("user") || !data["user"].is_object()) {
            return false;
        }

        const auto& user = data["user"];

        role = user.value("role", "");

        permissions.clear();

        if (user.contains("permissions") && user["permissions"].is_array()) {
            for (const auto& permission : user["permissions"]) {
                if (permission.is_string()) {
                    permissions.push_back(permission.get<std::string>());
                }
            }
        }

        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

bool ApiClient::refreshAccessToken(const std::string& usedToken)
{
    // Single-flight: only one refresh runs at a time.
    std::lock_guard<std::mutex> refreshLock(refreshMutex);

    std::string currentRefresh;
    {
        std::lock_guard<std::mutex> lock(tokenMutex);

        // A concurrent request already refreshed the token after this request
        // was sent, so the caller can simply retry with the new token.
        if (!accessToken.empty() && accessToken != usedToken) {
            return true;
        }

        if (refreshToken.empty()) {
            return false;
        }

        currentRefresh = refreshToken;
    }

    json body = {
        {"refreshToken", currentRefresh}
    };

    auto res = cpr::Post(
        cpr::Url{ baseUrl + "/auth/refresh" },
        cpr::Header{
            {"Content-Type", "application/json"}
        },
        cpr::Body{ body.dump() },
        cpr::Timeout{ kRequestTimeoutMs }
    );

    if (res.status_code != 200) {
        clearTokens();
        return false;
    }

    try {
        auto data = json::parse(res.text);

        std::string newAccess;
        if (data.contains("accessToken") && data["accessToken"].is_string()) {
            newAccess = data["accessToken"].get<std::string>();
        }
        else if (data.contains("token") && data["token"].is_string()) {
            newAccess = data["token"].get<std::string>();
        }

        if (newAccess.empty()) {
            clearTokens();
            return false;
        }

        std::lock_guard<std::mutex> lock(tokenMutex);
        accessToken = newAccess;

        // Honour refresh-token rotation if the server issues a new one.
        if (data.contains("refreshToken") && data["refreshToken"].is_string()) {
            refreshToken = data["refreshToken"].get<std::string>();
        }

        return true;
    }
    catch (const std::exception& ex) {
        inv::logWarning(std::string("Token refresh parse error: ") + ex.what());
        clearTokens();
        return false;
    }
}
