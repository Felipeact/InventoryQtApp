#include "UserService.h"
#include "Logging.h"

#include <algorithm>

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
            inv::logWarning(
                "GET /users failed. Status: "
                + std::to_string(response.status_code)
                + " Body: " + response.text
            );

            return users;
        }

        json parsed = json::parse(response.text);

        // The API may return a bare array or a { "data": [...] } envelope.
        const json& data =
            (parsed.contains("data") && parsed["data"].is_array())
            ? parsed["data"]
            : parsed;

        if (!data.is_array()) {
            return users;
        }

        for (const auto& item : data) {
            if (!item.is_object()) {
                continue;
            }

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
        inv::logWarning(std::string("UserService::getUsers error: ") + ex.what());
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
            inv::logWarning(
                "Invite user failed. Status: "
                + std::to_string(response.status_code)
                + " Body: " + response.text
            );

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
        inv::logWarning(std::string("UserService::inviteUser error: ") + ex.what());

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
            inv::logWarning(
                "PUT /users failed. Status: "
                + std::to_string(response.status_code)
                + " Body: " + response.text
            );

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        inv::logWarning(std::string("UserService::updateUser error: ") + ex.what());

        return false;
    }
}

bool UserService::deleteUser(const std::string& userId)
{
    try {
        auto response = apiClient.del("/users/" + userId);

        if (response.status_code != 200 && response.status_code != 204) {
            inv::logWarning(
                "DELETE /users failed. Status: "
                + std::to_string(response.status_code)
                + " Body: " + response.text
            );

            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        inv::logWarning(std::string("UserService::deleteUser error: ") + ex.what());

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

        inv::logWarning(
            "Update current profile failed. Status: "
            + std::to_string(response.status_code)
            + " Body: " + response.text
        );

        return false;
    }
    catch (const std::exception& ex) {
        inv::logWarning(std::string("UserService::updateCurrentUserProfile error: ") + ex.what());

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
