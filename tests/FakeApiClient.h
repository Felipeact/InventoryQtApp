// FakeApiClient.h - In-memory IApiClient test double.
//
// Records every request made by a service and returns scripted responses, so
// service behavior can be verified without any network access.
#pragma once

#include "IApiClient.h"

#include <string>
#include <vector>

class FakeApiClient : public IApiClient
{
public:
    struct Request
    {
        std::string method;
        std::string endpoint;
        std::string body;
    };

    // Every request the service issued, in order.
    std::vector<Request> requests;

    // Response returned for the next call. Tests overwrite this before
    // exercising the service under test.
    HttpResponse response{ 200, "{}" };

    // --- Token management -------------------------------------------------
    void setAccessToken(const std::string& token) override { accessToken = token; }
    void setRefreshToken(const std::string& token) override { refreshToken = token; }
    std::string getAccessToken() const override { return accessToken; }
    std::string getRefreshToken() const override { return refreshToken; }
    void clearTokens() override { accessToken.clear(); refreshToken.clear(); tokensCleared = true; }

    // --- HTTP verbs -------------------------------------------------------
    HttpResponse get(const std::string& endpoint) override
    {
        return record("GET", endpoint, "");
    }
    HttpResponse post(const std::string& endpoint, const std::string& body) override
    {
        return record("POST", endpoint, body);
    }
    HttpResponse put(const std::string& endpoint, const std::string& body) override
    {
        return record("PUT", endpoint, body);
    }
    HttpResponse patch(const std::string& endpoint, const std::string& body) override
    {
        return record("PATCH", endpoint, body);
    }
    HttpResponse del(const std::string& endpoint) override
    {
        return record("DELETE", endpoint, "");
    }

    // --- Test helpers -----------------------------------------------------
    bool tokensCleared = false;

    const Request& lastRequest() const { return requests.back(); }
    int requestCount() const { return static_cast<int>(requests.size()); }

    void setResponse(long status, const std::string& text)
    {
        response.status_code = status;
        response.text = text;
    }

private:
    std::string accessToken;
    std::string refreshToken;

    HttpResponse record(const std::string& method, const std::string& endpoint, const std::string& body)
    {
        requests.push_back({ method, endpoint, body });
        return response;
    }
};
