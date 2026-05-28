#include "ApiClient.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

ApiClient::ApiClient(const std::string& baseUrl)
    : baseUrl(baseUrl)
{
}

void ApiClient::setAccessToken(const std::string& token)
{
    accessToken = token;
}

void ApiClient::setRefreshToken(const std::string& token)
{
    refreshToken = token;
}

std::string ApiClient::getAccessToken() const
{
    return accessToken;
}

std::string ApiClient::getRefreshToken() const
{
    return refreshToken;
}

cpr::Header ApiClient::authHeader() const
{
    if (accessToken.empty()) {
        return cpr::Header{};
    }

    return cpr::Header{
        {"Authorization", "Bearer " + accessToken}
    };
}

void ApiClient::clearTokens()
{
    accessToken.clear();
    refreshToken.clear();
}

cpr::Response ApiClient::get(const std::string& endpoint)
{
    std::cout << "GET: " << baseUrl + endpoint << std::endl;
    std::cout << "ACCESS TOKEN EMPTY: " << (accessToken.empty() ? "YES" : "NO") << std::endl;

    auto res = cpr::Get(
        cpr::Url{ baseUrl + endpoint },
        authHeader()
    );

    std::cout << "STATUS: " << res.status_code << std::endl;
    std::cout << "BODY: " << res.text << std::endl;

    if (res.status_code == 401 && refreshAccessToken()) {
        res = cpr::Get(
            cpr::Url{ baseUrl + endpoint },
            authHeader()
        );
    }

    return res;
}

cpr::Response ApiClient::post(const std::string& endpoint, const std::string& body)
{
    std::cout << "POST: " << baseUrl + endpoint << std::endl;
    std::cout << "BODY SENT: " << body << std::endl;
    std::cout << "ACCESS TOKEN EMPTY: " << (accessToken.empty() ? "YES" : "NO") << std::endl;

    cpr::Header headers{
        {"Content-Type", "application/json"}
    };

    if (
        endpoint != "/auth/login" &&
        endpoint != "/auth/refresh" &&
        !accessToken.empty()
        ) {
        headers["Authorization"] = "Bearer " + accessToken;
    }

    auto res = cpr::Post(
        cpr::Url{ baseUrl + endpoint },
        headers,
        cpr::Body{ body }
    );

    std::cout << "STATUS: " << res.status_code << std::endl;
    std::cout << "BODY: " << res.text << std::endl;

    if (
        res.status_code == 401 &&
        endpoint != "/auth/login" &&
        endpoint != "/auth/refresh" &&
        refreshAccessToken()
        ) {
        cpr::Header retryHeaders{
            {"Content-Type", "application/json"},
            {"Authorization", "Bearer " + accessToken}
        };

        res = cpr::Post(
            cpr::Url{ baseUrl + endpoint },
            retryHeaders,
            cpr::Body{ body }
        );
    }

    return res;
}

cpr::Response ApiClient::put(const std::string& endpoint, const std::string& body)
{
    std::cout << "PUT: " << baseUrl + endpoint << std::endl;
    std::cout << "BODY SENT: " << body << std::endl;

    auto res = cpr::Put(
        cpr::Url{ baseUrl + endpoint },
        cpr::Header{
            {"Content-Type", "application/json"},
            {"Authorization", "Bearer " + accessToken}
        },
        cpr::Body{ body }
    );

    std::cout << "STATUS: " << res.status_code << std::endl;
    std::cout << "BODY: " << res.text << std::endl;

    if (res.status_code == 401 && refreshAccessToken()) {
        res = cpr::Put(
            cpr::Url{ baseUrl + endpoint },
            cpr::Header{
                {"Content-Type", "application/json"},
                {"Authorization", "Bearer " + accessToken}
            },
            cpr::Body{ body }
        );
    }

    return res;
}

cpr::Response ApiClient::patch(const std::string& endpoint, const std::string& body)
{
    std::cout << "PATCH: " << baseUrl + endpoint << std::endl;
    std::cout << "BODY SENT: " << body << std::endl;

    auto res = cpr::Patch(
        cpr::Url{ baseUrl + endpoint },
        cpr::Header{
            {"Content-Type", "application/json"},
            {"Authorization", "Bearer " + accessToken}
        },
        cpr::Body{ body }
    );

    std::cout << "STATUS: " << res.status_code << std::endl;
    std::cout << "BODY: " << res.text << std::endl;

    if (res.status_code == 401 && refreshAccessToken()) {
        res = cpr::Patch(
            cpr::Url{ baseUrl + endpoint },
            cpr::Header{
                {"Content-Type", "application/json"},
                {"Authorization", "Bearer " + accessToken}
            },
            cpr::Body{ body }
        );
    }

    return res;
}

cpr::Response ApiClient::del(const std::string& endpoint)
{
    std::cout << "DELETE: " << baseUrl + endpoint << std::endl;

    auto res = cpr::Delete(
        cpr::Url{ baseUrl + endpoint },
        authHeader()
    );

    std::cout << "STATUS: " << res.status_code << std::endl;
    std::cout << "BODY: " << res.text << std::endl;

    if (res.status_code == 401 && refreshAccessToken()) {
        res = cpr::Delete(
            cpr::Url{ baseUrl + endpoint },
            authHeader()
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

        role = data["user"]["role"].get<std::string>();

        permissions.clear();

        if (data["user"].contains("permissions")) {
            for (const auto& permission : data["user"]["permissions"]) {
                permissions.push_back(permission.get<std::string>());
            }
        }

        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

bool ApiClient::refreshAccessToken()
{
    if (refreshToken.empty()) {
        return false;
    }

    json body = {
        {"refreshToken", refreshToken}
    };

    auto res = cpr::Post(
        cpr::Url{ baseUrl + "/auth/refresh" },
        cpr::Header{
            {"Content-Type", "application/json"}
        },
        cpr::Body{ body.dump() }
    );

    if (res.status_code != 200) {
        clearTokens();
        return false;
    }

    auto data = json::parse(res.text);

    if (data.contains("accessToken")) {
        accessToken = data["accessToken"].get<std::string>();
        return true;
    }

    if (data.contains("token")) {
        accessToken = data["token"].get<std::string>();
        return true;
    }

    clearTokens();
    return false;
}