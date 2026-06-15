// ApiClient.h - HTTP client for API communication
#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <cpr/cpr.h>

// HTTP client for making API requests with token-based authentication.
//
// Thread-safety: instances are shared across the UI thread and the background
// worker threads that pages spawn for network calls. All access to the cached
// tokens is therefore serialized with tokenMutex, and token refresh is
// single-flighted with refreshMutex so concurrent 401s trigger at most one
// refresh.
class ApiClient
{
private:
	std::string baseUrl;        // Base URL for API endpoints (immutable after construction)
	std::string accessToken;    // Guarded by tokenMutex
	std::string refreshToken;   // Guarded by tokenMutex

	mutable std::mutex tokenMutex;  // Protects accessToken / refreshToken
	std::mutex refreshMutex;        // Serializes refreshAccessToken (single-flight)

	// Builds an Authorization header for the supplied bearer token.
	static cpr::Header makeAuthHeader(const std::string& token);

	// Refreshes the access token, but only if it still matches the token that
	// produced the 401 (avoids redundant refreshes when several requests race).
	bool refreshAccessToken(const std::string& usedToken);

public:
	// Constructor with base URL
	explicit ApiClient(const std::string& baseUrl);

	// Sets the authentication token for subsequent requests
	void setAccessToken(const std::string& token);
	void setRefreshToken(const std::string& token);

	std::string getAccessToken() const;
	std::string getRefreshToken() const;

	void clearTokens(); // Clears the stored access and refresh tokens

	cpr::Response get(const std::string& endpoint); // Makes a GET request to the specified endpoint
	cpr::Response post(const std::string& endpoint, const std::string& body); // Makes a POST request to the specified endpoint
	cpr::Response put(const std::string& endpoint, const std::string& body); // Makes a PUT request to the specified endpoint
	cpr::Response patch(const std::string& endpoint, const std::string& body);
	cpr::Response del(const std::string& endpoint);  // Makes a DELETE request to the specified endpoint

	// Validates the current token and retrieves user role and permissions
	bool validateToken(std::string& role, std::vector<std::string>& permissions);
};
