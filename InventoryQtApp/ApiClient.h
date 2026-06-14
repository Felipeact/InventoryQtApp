// ApiClient.h - HTTP client for API communication
#pragma once
#include "IApiClient.h"
#include <string>
#include <vector>
#include <cpr/cpr.h>

// HTTP client for making API requests with token-based authentication.
// Concrete implementation of IApiClient backed by the cpr library; cpr is an
// implementation detail and is not exposed through the public interface.
class ApiClient : public IApiClient
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
	void setAccessToken(const std::string& token) override;
	void setRefreshToken(const std::string& token) override;

	std::string getAccessToken() const override;
	std::string getRefreshToken() const override;

	void clearTokens() override; // Clears the stored access and refresh tokens

	HttpResponse get(const std::string& endpoint) override; // Makes a GET request to the specified endpoint
	HttpResponse post(const std::string& endpoint, const std::string& body) override; // Makes a POST request to the specified endpoint
	HttpResponse put(const std::string& endpoint, const std::string& body) override; // Makes a PUT request to the specified endpoint
	HttpResponse patch(const std::string& endpoint, const std::string& body) override;
	HttpResponse del(const std::string& endpoint) override;  // Makes a DELETE request to the specified endpoint

	// Validates the current token and retrieves user role and permissions
	bool validateToken(std::string& role, std::vector<std::string>& permissions);

	bool refreshAccessToken(); // Refreshes the access token using the refresh token

};
