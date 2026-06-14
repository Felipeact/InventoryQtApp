#pragma once

#include "IApiClient.h"

#include <string>
#include <vector>

struct UserDto
{
    std::string id;
    std::string name;
    std::string email;
    std::string role;
    std::string status;
    std::string createdAt;
};

struct CreateUserRequest
{
    std::string name;
    std::string email;
    std::string password;
    std::string role;
    std::string status;
};

struct UpdateUserRequest
{
    std::string name;
    std::string email;
    std::string password;
    std::string role;
    std::string status;
};

class UserService
{
public:
    explicit UserService(IApiClient& apiClient);

    std::vector<UserDto> getUsers();
    std::vector<UserDto> getTechnicians();

    std::string inviteUser(const CreateUserRequest& request);
    bool createUser(const CreateUserRequest& request);
    bool updateUser(const std::string& userId, const UpdateUserRequest& request);
    bool deleteUser(const std::string& userId);

    bool updateCurrentUserProfile(const std::string& name);

    std::string resetPassword(
        const std::string& userId
    );

private:
    IApiClient& apiClient;
};