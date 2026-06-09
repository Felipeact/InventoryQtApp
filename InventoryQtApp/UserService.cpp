#include "UserService.h"

#include <algorithm>
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

UserService::UserService(ApiClient& apiClient)
    : apiClient(apiClient)
{
}

std::vector<UserDto> UserService::getUsers()
{
    std::vector<UserDto> users;

    try {
        auto response = apiClient.get("/users");

        if (response.status_code != 200) {
            std::cerr << "GET /users failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return users;
        }

        json data = json::parse(response.text);

        for (const auto& item : data) {
            UserDto user;

            user.id = item.value("id", "");
            user.name = item.value("name", "");
            user.email = item.value("email", "");
            user.status = item.value("status", "Active");
            user.createdAt = item.value("createdAt", "");

            if (item.contains("role") && item["role"].is_object()) {
                user.role = item["role"].value("name", "");
            }
            else {
                user.role = item.value("role", "");
            }

            users.push_back(user);
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "UserService::getUsers error: "
            << ex.what()
            << std::endl;
    }

    return users;
}

std::vector<UserDto> UserService::getTechnicians()
{
    std::vector<UserDto> technicians;
    std::vector<UserDto> users = getUsers();

    for (const UserDto& user : users) {
        if (user.role == "TECHNICIAN") {
            technicians.push_back(user);
        }
    }

    return technicians;
}

std::string UserService::inviteUser(const CreateUserRequest& request)
{
    try {
        json body;

        body["name"] = request.name;
        body["email"] = request.email;
        body["role"] = request.role;
        body["status"] = request.status;

        auto response = apiClient.post(
            "/users/invite",
            body.dump()
        );

        if (response.status_code != 200 && response.status_code != 201) {
            std::cerr << "Invite user failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return "";
        }

        auto data = json::parse(response.text);

        bool emailSent = data.value("emailSent", false);

        if (emailSent) {
            return "__EMAIL_SENT__";
        }

        return data.value("temporaryPassword", "");
    }
    catch (const std::exception& ex) {
        std::cerr << "UserService::inviteUser error: "
            << ex.what()
            << std::endl;

        return "";
    }
}

bool UserService::updateUser(
    const std::string& userId,
    const UpdateUserRequest& request
)
{
    try {
        json body;

        body["name"] = request.name;
        body["email"] = request.email;
        body["role"] = request.role;
        body["status"] = request.status;

        if (!request.password.empty()) {
            body["password"] = request.password;
        }

        auto response = apiClient.put("/users/" + userId, body.dump());

        if (response.status_code != 200) {
            std::cerr << "PUT /users failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "UserService::updateUser error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool UserService::deleteUser(const std::string& userId)
{
    try {
        auto response = apiClient.del("/users/" + userId);

        if (response.status_code != 200 && response.status_code != 204) {
            std::cerr << "DELETE /users failed. Status: "
                << response.status_code
                << " Body: "
                << response.text
                << std::endl;

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "UserService::deleteUser error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

bool UserService::updateCurrentUserProfile(const std::string& name)
{
    try {
        json body;
        body["name"] = name;

        auto response = apiClient.patch(
            "/auth/profile",
            body.dump()
        );

        if (response.status_code == 200 || response.status_code == 204) {
            return true;
        }

        response = apiClient.patch(
            "/users/me",
            body.dump()
        );

        if (response.status_code == 200 || response.status_code == 204) {
            return true;
        }

        std::cerr << "Update current profile failed. Status: "
            << response.status_code
            << " Body: "
            << response.text
            << std::endl;

        return false;
    }
    catch (const std::exception& ex) {
        std::cerr << "UserService::updateCurrentUserProfile error: "
            << ex.what()
            << std::endl;

        return false;
    }
}

std::string UserService::resetPassword(
    const std::string& userId
)
{
    try {

        auto response =
            apiClient.post(
                "/users/" + userId + "/reset-password",
                "{}"
            );

        if (response.status_code != 200) {
            return "";
        }

        auto jsonResponse =
            json::parse(response.text);

        return jsonResponse.value(
            "temporaryPassword",
            ""
        );
    }
    catch (...) {
        return "";
    }
}