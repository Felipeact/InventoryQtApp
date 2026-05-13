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
	std::string accessToken; // Authentication token for API requests
	std::string refreshToken; // Refresh token for obtaining new access tokens

	cpr::Header authHeader() const; // Helper function to construct the Authorization header

public:
	// Constructor with base URL
	ApiClient(const std::string& baseUrl);

	// Sets the authentication token for subsequent requests
	void setAccessToken(const std::string& token);
	void setRefreshToken(const std::string& token);

	std::string getAccessToken() const;
	std::string getRefreshToken() const;

	void clearTokens(); // Clears the stored access and refresh tokens

	cpr::Response get(const std::string& endpoint); // Makes a GET request to the specified endpoint
	cpr::Response post(const std::string& endpoint, const std::string& body); // Makes a POST request to the specified endpoint
	cpr::Response put(const std::string& endpoint, const std::string& body); // Makes a PUT request to the specified endpoint
	cpr::Response del(const std::string& endpoint);  // Makes a DELETE request to the specified endpoint

	// Validates the current token and retrieves user role and permissions
	bool validateToken(std::string& role, std::vector<std::string>& permissions);
	
	bool refreshAccessToken(); // Refreshes the access token using the refresh token
			
};
