#include "ApiClient.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

ApiClient::ApiClient(const std::string& baseUrl) :
	baseUrl(baseUrl)
{
}

void ApiClient::setToken(const std::string& newToken)
{
	token = newToken;
}

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

cpr::Response ApiClient::get(const std::string& endpoint)
{
	return cpr::Get(
		cpr::Url{ baseUrl + endpoint },
		cpr::Header{
			{"Authorization", "Bearer " + token}
		}
	);
}
