#pragma once

#include <string>
#include <cpr/cpr.h>

class ErrorHandler
{
public:
    // Get user-friendly error message from HTTP response
    static std::string getErrorMessage(const cpr::Response& response);

    // Check if error is due to server being unavailable
    static bool isServerUnavailable(const cpr::Response& response);

    // Check if error is due to network/connection issue
    static bool isNetworkError(const cpr::Response& response);

    // Check if error is due to authentication failure
    static bool isAuthenticationError(const cpr::Response& response);

    // Get detailed error information
    static std::string getDetailedErrorInfo(const cpr::Response& response);
};
