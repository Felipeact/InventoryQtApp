// AuthService.h - Authentication service for user login
#pragma once
#include <string>

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
	std::string login(const std::string& email, const std::string& password);
	


};