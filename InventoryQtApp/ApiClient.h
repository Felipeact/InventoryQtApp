#pragma once
#include <string>
#include <vector>
#include <cpr/cpr.h>


class ApiClient
{
private:
	std::string baseUrl;
	std::string token;

public:
	ApiClient(const std::string& baseUrl);

	void setToken(const std::string& newToken);

	bool validateToken(std::string& role, std::vector<std::string>& permissions);

	cpr::Response post(const std::string& endpoint, const std::string& body);

	cpr::Response get(const std::string& endpoint);
		
};
