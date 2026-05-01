#pragma once
#include <string>

class ApiClient;

class AuthService
{
private:
	ApiClient& api;

public:
	AuthService(ApiClient& apiClient);

	std::string login(const std::string& email, const std::string& password);
	


};