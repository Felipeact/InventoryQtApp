#include "ErrorHandler.h"

std::string ErrorHandler::getErrorMessage(const cpr::Response& response)
{
    if (response.status_code == 0) {
        // Network error or connection refused
        return "Unable to connect to the server. Please check your internet connection.";
    }

    switch (response.status_code) {
        case 200:
        case 201:
        case 204:
            return "Success";

        case 400:
            return "Bad Request: The server could not understand the request.";

        case 401:
            return "Unauthorized: Your session has expired. Please login again.";

        case 403:
            return "Forbidden: You do not have permission to access this resource.";

        case 404:
            return "Not Found: The requested resource was not found on the server.";

        case 408:
            return "Request Timeout: The server took too long to respond. Please try again.";

        case 429:
            return "Too Many Requests: Please wait a moment before trying again.";

        case 500:
            return "Server Error: An internal server error occurred. Please try again later.";

        case 502:
            return "Bad Gateway: The server is temporarily unavailable. Please try again later.";

        case 503:
            return "Service Unavailable: The server is currently down for maintenance. Please try again later.";

        case 504:
            return "Gateway Timeout: The server is not responding. Please try again later.";

        default:
            if (response.status_code >= 500) {
                return "Server Error: The server encountered an error. Error code: " + std::to_string(response.status_code);
            } else if (response.status_code >= 400) {
                return "Request Error: An error occurred with your request. Error code: " + std::to_string(response.status_code);
            } else {
                return "An unexpected error occurred. Error code: " + std::to_string(response.status_code);
            }
    }
}

bool ErrorHandler::isServerUnavailable(const cpr::Response& response)
{
    return response.status_code == 0 || 
           response.status_code == 502 || 
           response.status_code == 503 || 
           response.status_code == 504;
}

bool ErrorHandler::isNetworkError(const cpr::Response& response)
{
    return response.status_code == 0;
}

bool ErrorHandler::isAuthenticationError(const cpr::Response& response)
{
    return response.status_code == 401 || response.status_code == 403;
}

std::string ErrorHandler::getDetailedErrorInfo(const cpr::Response& response)
{
    std::string info = "Status Code: " + std::to_string(response.status_code) + "\n";
    info += "User Message: " + getErrorMessage(response) + "\n";

    if (!response.text.empty()) {
        info += "Server Response: " + response.text;
    }

    if (isNetworkError(response)) {
        info += "\nTroubleshooting:\n";
        info += "1. Check your internet connection\n";
        info += "2. Verify the API server address in settings\n";
        info += "3. Check if firewall is blocking the connection";
    }

    return info;
}
