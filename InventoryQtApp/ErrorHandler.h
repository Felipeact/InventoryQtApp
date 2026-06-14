#pragma once

#include "HttpResponse.h"

#include <string>

class ErrorHandler
{
public:
    // Get user-friendly error message from HTTP response
    static std::string getErrorMessage(const HttpResponse& response);

    // Check if error is due to server being unavailable
    static bool isServerUnavailable(const HttpResponse& response);

    // Check if error is due to network/connection issue
    static bool isNetworkError(const HttpResponse& response);

    // Check if error is due to authentication failure
    static bool isAuthenticationError(const HttpResponse& response);

    // Get detailed error information
    static std::string getDetailedErrorInfo(const HttpResponse& response);
};
