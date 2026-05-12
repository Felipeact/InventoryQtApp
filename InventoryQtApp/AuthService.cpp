// AuthService.cpp - Implementation of authentication service
#include "AuthService.h"
#include "ApiClient.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Constructor initializes the service with an API client reference
AuthService::AuthService(ApiClient& apiClient) : api(apiClient)
{
}

// Sends login credentials to the server and returns the authentication token
LoginResult AuthService::login( const std::string& email, const std::string& password )
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