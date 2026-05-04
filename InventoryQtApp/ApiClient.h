// ApiClient.h - HTTP client for API communication
#pragma once
#include <string>
#include <vector>
#include <cpr/cpr.h>

// HTTP client for making API requests with token-based authentication
class ApiClient
{
private:
	std::string baseUrl;  // Base URL for API endpoints
	std::string token;    // Authentication token

public:
	// Constructor with base URL
	ApiClient(const std::string& baseUrl);

	// Sets the authentication token for subsequent requests
	void setToken(const std::string& newToken);

	// Validates the current token and retrieves user role and permissions
	bool validateToken(std::string& role, std::vector<std::string>& permissions);

	// Makes a POST request to the specified endpoint
	cpr::Response post(const std::string& endpoint, const std::string& body);

	// Makes a GET request to the specified endpoint
	cpr::Response get(const std::string& endpoint);
		
};
