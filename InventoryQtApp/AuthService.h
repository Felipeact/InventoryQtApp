// AuthService.h - Authentication service for user login
#pragma once
#include <string>

struct LoginResult
{
	bool success = false;
	std::string accessToken;
	std::string refreshToken;
	std::string userName;
	std::string errorMessage;
};

struct PasswordResetResult
{
	bool success = false;
	std::string message;
	std::string errorMessage;
};

class IApiClient;

// Service for handling user authentication and login operations
class AuthService
{
private:
	IApiClient& api;  // Reference to the API client

public:
	// Constructor that takes a reference to an ApiClient
	AuthService(IApiClient& apiClient);

	// Attempts to log in with the given email and password, returns authentication token
	LoginResult login(const std::string& email, const std::string& password);

	bool logout(); // Logs out the user by clearing tokens and session data

	// Request password reset - sends reset link to user's email
	PasswordResetResult requestPasswordReset(const std::string& email);

	// Verify reset token and set new password
	PasswordResetResult resetPassword(const std::string& token, const std::string& newPassword);
};
