#include "AuthService.h"
#include "ApiClient.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

AuthService::AuthService(ApiClient& apiClient) : api(apiClient)
{
}

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
}
