// AuthService.cpp - Implementation of authentication service
#include "AuthService.h"
#include "ApiClient.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static std::string readUserNameFromJson(const json& source, const std::string& fallbackEmail)
{
    if (!source.is_object()) {
        return fallbackEmail;
    }

    const char* directKeys[] = {
        "name",
        "fullName",
        "displayName",
        "userName",
        "username"
    };

    for (const char* key : directKeys) {
        if (source.contains(key) && source[key].is_string()) {
            std::string value = source[key].get<std::string>();
            if (!value.empty()) {
                return value;
            }
        }
    }

    std::string firstName;
    std::string lastName;

    if (source.contains("firstName") && source["firstName"].is_string()) {
        firstName = source["firstName"].get<std::string>();
    }

    if (source.contains("lastName") && source["lastName"].is_string()) {
        lastName = source["lastName"].get<std::string>();
    }

    std::string combinedName = firstName;
    if (!combinedName.empty() && !lastName.empty()) {
        combinedName += " ";
    }
    combinedName += lastName;

    if (!combinedName.empty()) {
        return combinedName;
    }

    if (source.contains("email") && source["email"].is_string()) {
        std::string email = source["email"].get<std::string>();
        if (!email.empty()) {
            return email;
        }
    }

    return fallbackEmail;
}


// Constructor initializes the service with an API client reference
AuthService::AuthService(ApiClient& apiClient) : api(apiClient)
{
}

// Sends login credentials to the server and returns the authentication token
LoginResult AuthService::login(const std::string& email, const std::string& password)
{
    LoginResult result;

    json body = {
        {"email", email},
        {"password", password}
    };

    auto response = api.post("/auth/login", body.dump());

    if (response.status_code != 200) {
        result.success = false;
        result.errorMessage = response.text;
        return result;
    }

    try {
        auto data = json::parse(response.text);

        if (!data.contains("accessToken") || !data.contains("refreshToken")) {
            result.success = false;
            result.errorMessage = "Login response missing tokens: " + response.text;
            return result;
        }

        result.success = true;
        result.accessToken = data["accessToken"].get<std::string>();
        result.refreshToken = data["refreshToken"].get<std::string>();

        if (data.contains("user") && data["user"].is_object()) {
            result.userName = readUserNameFromJson(data["user"], email);
        }
        else {
            result.userName = readUserNameFromJson(data, email);
        }

        return result;
    }
    catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = std::string("JSON parse error: ") + e.what()
            + "\nResponse: " + response.text;
        return result;
    }
}

bool AuthService::logout()
{
    if (api.getRefreshToken().empty()) {
        api.clearTokens();
        return true;
    }

    json body = {
        {"refreshToken", api.getRefreshToken()}
    };

    auto response = api.post("/auth/logout", body.dump());

    api.clearTokens();
    return response.status_code == 200;
}

PasswordResetResult AuthService::requestPasswordReset(const std::string& email)
{
    PasswordResetResult result;

    json body = {
        {"email", email}
    };

    auto response = api.post("/auth/request-reset", body.dump());

    if (response.status_code != 200) {
        result.success = false;
        result.errorMessage = response.text;
        return result;
    }

    try {
        auto data = json::parse(response.text);
        result.success = true;
        result.message = data.contains("message") ? data["message"].get<std::string>() : "Reset link sent to email";
        return result;
    }
    catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = std::string("JSON parse error: ") + e.what();
        return result;
    }
}

PasswordResetResult AuthService::resetPassword(const std::string& token, const std::string& newPassword)
{
    PasswordResetResult result;

    json body = {
        {"token", token},
        {"newPassword", newPassword}
    };

    auto response = api.post("/auth/reset-password", body.dump());

    if (response.status_code != 200) {
        result.success = false;
        result.errorMessage = response.text;
        return result;
    }

    try {
        auto data = json::parse(response.text);
        result.success = true;
        result.message = data.contains("message") ? data["message"].get<std::string>() : "Password reset successful";
        return result;
    }
    catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = std::string("JSON parse error: ") + e.what();
        return result;
    }
}