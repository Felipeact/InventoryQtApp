#include "ApiClient.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cpr/timeout.h>

using json = nlohmann::json;

namespace
{
    // Converts a cpr response into the transport-agnostic HttpResponse used by
    // the rest of the application. This is the single point where cpr types
    // cross into the application.
    HttpResponse toHttp(const cpr::Response& res)
    {
        HttpResponse out;
        out.status_code = res.status_code;
        out.text = res.text;
        return out;
    }
}

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

HttpResponse ApiClient::get(const std::string& endpoint)
{
    auto res = cpr::Get(
        cpr::Url{ baseUrl + endpoint },
        authHeader(),
        cpr::Timeout{ 5000 }
    );

    std::cout << "GET " << endpoint << " -> " << res.status_code << std::endl;

    if (res.status_code == 401 && refreshAccessToken()) {
        res = cpr::Get(
            cpr::Url{ baseUrl + endpoint },
            authHeader(),
            cpr::Timeout{ 5000 }
        );
    }

    return toHttp(res);
}

HttpResponse ApiClient::post(const std::string& endpoint, const std::string& body)
{


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
        cpr::Body{ body },
        cpr::Timeout{ 5000 }
    );

    std::cout << "POST " << endpoint << " -> " << res.status_code << std::endl;

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
            cpr::Body{ body },
            cpr::Timeout{ 5000 }
        );
    }

    return toHttp(res);
}

HttpResponse ApiClient::put(const std::string& endpoint, const std::string& body)
{
    auto res = cpr::Put(
        cpr::Url{ baseUrl + endpoint },
        cpr::Header{
            {"Content-Type", "application/json"},
            {"Authorization", "Bearer " + accessToken}
        },
        cpr::Body{ body },
        cpr::Timeout{ 5000 }
    );

    std::cout << "PUT " << endpoint << " -> " << res.status_code << std::endl;

    if (res.status_code == 401 && refreshAccessToken()) {
        res = cpr::Put(
            cpr::Url{ baseUrl + endpoint },
            cpr::Header{
                {"Content-Type", "application/json"},
                {"Authorization", "Bearer " + accessToken}
            },
            cpr::Body{ body },
            cpr::Timeout{ 5000 }
        );
    }

    return toHttp(res);
}

HttpResponse ApiClient::patch(const std::string& endpoint, const std::string& body)
{
    auto res = cpr::Patch(
        cpr::Url{ baseUrl + endpoint },
        cpr::Header{
            {"Content-Type", "application/json"},
            {"Authorization", "Bearer " + accessToken}
        },
        cpr::Body{ body },
        cpr::Timeout{ 5000 }
    );

    std::cout << "PATCH " << endpoint << " -> " << res.status_code << std::endl;

    if (res.status_code == 401 && refreshAccessToken()) {
        res = cpr::Patch(
            cpr::Url{ baseUrl + endpoint },
            cpr::Header{
                {"Content-Type", "application/json"},
                {"Authorization", "Bearer " + accessToken}
            },
            cpr::Body{ body },
            cpr::Timeout{ 5000 }
        );
    }

    return toHttp(res);
}

HttpResponse ApiClient::del(const std::string& endpoint)
{
    auto res = cpr::Delete(
        cpr::Url{ baseUrl + endpoint },
        authHeader(),
        cpr::Timeout{ 5000 }
    );

    std::cout << "DELETE " << endpoint << " -> " << res.status_code << std::endl;

    if (res.status_code == 401 && refreshAccessToken()) {
        res = cpr::Delete(
            cpr::Url{ baseUrl + endpoint },
            authHeader(),
            cpr::Timeout{ 5000 }
        );
    }

    return toHttp(res);
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
        cpr::Body{ body.dump() },
        cpr::Timeout{ 5000 }
    );

    if (res.status_code != 200) {
        clearTokens();
        return false;
    }

    try {
        auto data = json::parse(res.text);

        if (data.contains("accessToken")) {
            accessToken = data["accessToken"].get<std::string>();
            return true;
        }

        if (data.contains("token")) {
            accessToken = data["token"].get<std::string>();
            return true;
        }
    }
    catch (const std::exception&) {
        // Fall through to clearing tokens on malformed payloads.
    }

    clearTokens();
    return false;
}
