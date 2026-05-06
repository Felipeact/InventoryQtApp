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
void ApiClient::setToken(const std::string& newToken)
{
	token = newToken;
}

// Validates the token and retrieves user role and permissions from the server
bool ApiClient::validateToken(std::string& role, std::vector<std::string>& permissions)
{
	auto res = cpr::Get(
		cpr::Url{ baseUrl + "/auth/validate" },
		cpr::Header{
			{"Authorization", "Bearer " + token}
		}
	);

	if (res.status_code == 401) {
		//std::cout << "Session expired\n";
		return false;
	}

	if (res.status_code != 200) return false;

	auto data = json::parse(res.text);
	role = data["user"]["role"];

	permissions.clear();
	for (const auto& p : data["user"]["permissions"]) {
		permissions.push_back(p);
	}

	return true;
}

// Sends a POST request with the given endpoint and JSON body
cpr::Response ApiClient::post(const std::string& endpoint, const std::string& body)
{
	return cpr::Post(
		cpr::Url{ baseUrl + endpoint },
		cpr::Header{
			{"Content-Type", "application/json"},
			{"Authorization", "Bearer " + token}
		},
		cpr::Body{ body }
	);
}

// Sends a GET request to the specified endpoint
cpr::Response ApiClient::get(const std::string& endpoint)
{
	return cpr::Get(
		cpr::Url{ baseUrl + endpoint },
		cpr::Header{
			{"Authorization", "Bearer " + token}
		}
	);
}

cpr::Response ApiClient::put(const std::string& endpoint, const std::string& body)
{
	return cpr::Put(
		cpr::Url{ baseUrl + endpoint },
		cpr::Header{
			{"Content-Type", "application/json"},
			{"Authorization", "Bearer " + token}
		},
		cpr::Body{ body }
	);
}

cpr::Response ApiClient::del(const std::string& endpoint)
{
	return cpr::Delete(
		cpr::Url{ baseUrl + endpoint },
		cpr::Header{
			{"Authorization", "Bearer " + token}
		}
	);
}
