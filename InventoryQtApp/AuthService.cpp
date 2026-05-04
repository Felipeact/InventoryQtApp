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
std::string AuthService::login(const std::string& email, const std::string& password)
{
	json body = {
			{"email", email},
			{"password", password}
	};



	auto response = api.post("/auth/login", body.dump());

	if (response.status_code != 200) {
		return "";
	}

	auto data = json::parse(response.text);

	if (!data.contains("token")) return "";

	return data["token"];
} // Returns token extracted from response
