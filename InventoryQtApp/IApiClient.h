// IApiClient.h - Abstract HTTP client interface.
//
// Services depend on this interface rather than on the concrete ApiClient.
// This provides a seam for unit testing (a fake implementation can return
// canned responses without any network access) and keeps the cpr dependency
// confined to ApiClient.cpp.
#pragma once

#include "HttpResponse.h"

#include <string>

class IApiClient
{
public:
    virtual ~IApiClient() = default;

    // Token management.
    virtual void setAccessToken(const std::string& token) = 0;
    virtual void setRefreshToken(const std::string& token) = 0;
    virtual std::string getAccessToken() const = 0;
    virtual std::string getRefreshToken() const = 0;
    virtual void clearTokens() = 0;

    // HTTP verbs.
    virtual HttpResponse get(const std::string& endpoint) = 0;
    virtual HttpResponse post(const std::string& endpoint, const std::string& body) = 0;
    virtual HttpResponse put(const std::string& endpoint, const std::string& body) = 0;
    virtual HttpResponse patch(const std::string& endpoint, const std::string& body) = 0;
    virtual HttpResponse del(const std::string& endpoint) = 0;
};
