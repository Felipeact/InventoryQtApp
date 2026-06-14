// JsonUtils.h - Pure, dependency-light helpers for normalizing API JSON.
//
// These functions were previously buried in anonymous namespaces / static
// functions inside individual services, which made them impossible to test in
// isolation. They depend only on nlohmann/json so they can be exercised
// directly by unit tests.
#pragma once

#include <nlohmann/json.hpp>

#include <string>

namespace JsonUtils
{
    using json = nlohmann::json;

    // Returns item[key] as a string, falling back to `fallback` when the key
    // is missing, null, or not a string. Never throws.
    inline std::string safeString(
        const json& item,
        const std::string& key,
        const std::string& fallback = ""
    )
    {
        if (!item.is_object() || !item.contains(key) || item[key].is_null()) {
            return fallback;
        }

        if (item[key].is_string()) {
            return item[key].get<std::string>();
        }

        return fallback;
    }

    // Normalizes a single asset object so every consumer can rely on a stable
    // set of string fields being present.
    inline json normalizeAsset(const json& item)
    {
        json asset = item;

        asset["id"] = safeString(item, "id");
        asset["name"] = safeString(item, "name");
        asset["type"] = safeString(item, "type");
        asset["serialCode"] = safeString(item, "serialCode");
        asset["status"] = safeString(item, "status", "Active");
        asset["description"] = safeString(item, "description");
        asset["createdAt"] = safeString(item, "createdAt");
        asset["updatedAt"] = safeString(item, "updatedAt");

        return asset;
    }

    // Accepts either a `{ "data": [...] }` envelope or a bare array and returns
    // a normalized array of asset objects. Non-object entries are skipped.
    inline json normalizeAssetsResponse(const json& response)
    {
        json assets = json::array();
        json data = json::array();

        if (response.is_object() && response.contains("data") && response["data"].is_array()) {
            data = response["data"];
        }
        else if (response.is_array()) {
            data = response;
        }

        for (const auto& item : data) {
            if (item.is_object()) {
                assets.push_back(normalizeAsset(item));
            }
        }

        return assets;
    }

    // Resolves a human-friendly display name from a user object, following a
    // priority chain of direct name fields, then first/last name, then email,
    // and finally the supplied fallback.
    inline std::string readUserNameFromJson(
        const json& source,
        const std::string& fallbackEmail
    )
    {
        if (!source.is_object()) {
            return fallbackEmail;
        }

        const char* directKeys[] = {
            "name",
            "fullName",
            "displayName",
            "userName",
            "username"
        };

        for (const char* key : directKeys) {
            if (source.contains(key) && source[key].is_string()) {
                std::string value = source[key].get<std::string>();
                if (!value.empty()) {
                    return value;
                }
            }
        }

        std::string firstName;
        std::string lastName;

        if (source.contains("firstName") && source["firstName"].is_string()) {
            firstName = source["firstName"].get<std::string>();
        }

        if (source.contains("lastName") && source["lastName"].is_string()) {
            lastName = source["lastName"].get<std::string>();
        }

        std::string combinedName = firstName;
        if (!combinedName.empty() && !lastName.empty()) {
            combinedName += " ";
        }
        combinedName += lastName;

        if (!combinedName.empty()) {
            return combinedName;
        }

        if (source.contains("email") && source["email"].is_string()) {
            std::string email = source["email"].get<std::string>();
            if (!email.empty()) {
                return email;
            }
        }

        return fallbackEmail;
    }
}
