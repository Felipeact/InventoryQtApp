// ApiClient.cpp - Implementation of HTTP client for API communication
#include "ApiClient.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Constructor initializes the base URL for all API requests
ApiClient::ApiClient(const std::string& baseUrl) :
	baseUrl(baseUrl)
{
}

// Updates the authentication token for subsequent API requests
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

// Clears the stored access and refresh tokens, effectively logging out the user
void ApiClient::clearTokens()
{
	accessToken.clear();
	refreshToken.clear();
}


// Sends a GET request to the specified endpoint
cpr::Response ApiClient::get(const std::string& endpoint)
{
	return cpr::Get(
		cpr::Url{ baseUrl + endpoint },
		cpr::Header{
			{"Authorization", "Bearer " + accessToken}
		}
	);
}

// Sends a POST request with the given endpoint and JSON body
cpr::Response ApiClient::post(const std::string& endpoint, const std::string& body)
{
	return cpr::Post(
		cpr::Url{ baseUrl + endpoint },
		cpr::Header{
			{"Content-Type", "application/json"},
			{"Authorization", "Bearer " + accessToken}
		},
		cpr::Body{ body }
	);
}

cpr::Response ApiClient::put(const std::string& endpoint, const std::string& body)
{
	return cpr::Put(
		cpr::Url{ baseUrl + endpoint },
		cpr::Header{
			{"Content-Type", "application/json"},
			{"Authorization", "Bearer " + accessToken}
		},
		cpr::Body{ body }
	);
}

cpr::Response ApiClient::del(const std::string& endpoint)
{
	return cpr::Delete(
		cpr::Url{ baseUrl + endpoint },
		cpr::Header{
			{"Authorization", "Bearer " + accessToken}
		}
	);
}




// Validates the token and retrieves user role and permissions from the server
bool ApiClient::validateToken(std::string& role, std::vector<std::string>& permissions)
{
	try
	{
		auto res = get("/auth/validate");

		if (res.status_code != 200) {
			return false;
		}

		auto data = json::parse(res.text);

		role = data["user"]["role"];

		permissions.clear();

		if (data["user"].contains("permissions")) {
			for (const auto& permission : data["user"]["permissions"]) {
				permissions.push_back(permission.get<std::string>());
			}
		}

		return true;
	}
	catch (const std::exception&)
	{

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
		cpr::Body{ body.dump() }
	);

	if (res.status_code != 200) {
		return false;
	}

	auto data = json::parse(res.text);

	if (!data.contains("accessToken")) {
		return false;
	}

	accessToken = data["accessToken"].get<std::string>();

	return true;
}





