// AuthService.h - Authentication service for user login
#pragma once
#include <string>

struct LoginResult
{
	bool success; // Indicates if login was successful
	std::string accessToken; // Authentication token returned on successful login
	std::string refreshToken; // Refresh token returned on successful login	
	std::string errorMessage; // Error message in case of failed login
};

class ApiClient;

// Service for handling user authentication and login operations
class AuthService
{
private:
	ApiClient& api;  // Reference to the API client

public:
	// Constructor that takes a reference to an ApiClient
	AuthService(ApiClient& apiClient);

	// Attempts to log in with the given email and password, returns authentication token
	LoginResult login(const std::string& email, const std::string& password);
	
	bool logout(); // Logs out the user by clearing tokens and session data


};