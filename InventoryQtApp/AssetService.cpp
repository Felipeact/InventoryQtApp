// AssetService.cpp - Implementation of asset service
#include "AssetService.h"

#include <iostream>

AssetService::AssetService(ApiClient& apiClient)
    : api(apiClient)
{
}

bool AssetService::createAsset(
    const std::string& name,
    const std::string& type,
    const std::string& serialCode,
    const std::string& status,
    const std::string& description
)
{
    json body = {
        {"name", name},
        {"type", type},
        {"serialCode", serialCode},
        {"status", status},
        {"description", description}
    };

    auto res = api.post("/assets", body.dump());

    if (res.status_code != 200 && res.status_code != 201) {
        std::cout << "Create asset failed: " << res.text << std::endl;
        return false;
    }

    cachedAssets = json::array();
    return true;
}

json AssetService::getAssets(bool forceRefresh)
{
    Q_UNUSED(forceRefresh);

    auto res = api.get("/assets?page=1&limit=100");

    if (res.status_code != 200) {
        std::cout << "Get assets failed: " << res.text << std::endl;
        return json::array();
    }

    try {
        auto response = json::parse(res.text);

        if (response.contains("data") && response["data"].is_array()) {
            return response["data"];
        }

        if (response.is_array()) {
            return response;
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Parse assets failed: " << ex.what() << std::endl;
    }

    return json::array();
}

bool AssetService::updateAsset(
    const std::string& assetId,
    const std::string& name,
    const std::string& type,
    const std::string& serialCode,
    const std::string& status,
    const std::string& description
)
{
    json body = {
        {"name", name},
        {"type", type},
        {"serialCode", serialCode},
        {"status", status},
        {"description", description}
    };

    auto res = api.put("/assets/" + assetId, body.dump());

    if (res.status_code != 200) {
        std::cout << "Update asset failed: " << res.text << std::endl;
        return false;
    }

    cachedAssets = json::array();
    return true;
}

bool AssetService::deleteAsset(const std::string& assetId)
{
    auto res = api.del("/assets/" + assetId);

    if (res.status_code != 200) {
        std::cout << "Delete asset failed: " << res.text << std::endl;
        return false;
    }

    cachedAssets = json::array();
    return true;
}

json AssetService::searchAssets(const std::string& searchText)
{
    std::string endpoint = "/assets?page=1&limit=100";

    if (!searchText.empty()) {
        endpoint += "&search=" + searchText;
    }

    auto res = api.get(endpoint);

    if (res.status_code != 200) {
        std::cout << "Search assets failed: " << res.text << std::endl;
        return json::array();
    }

    try {
        auto response = json::parse(res.text);

        if (response.contains("data") && response["data"].is_array()) {
            return response["data"];
        }

        if (response.is_array()) {
            return response;
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Parse search assets failed: " << ex.what() << std::endl;
    }

    return json::array();
}