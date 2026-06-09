// AssetService.cpp - Implementation of asset service
#include "AssetService.h"

#include <iostream>

namespace
{
    std::string safeString(
        const json& item,
        const std::string& key,
        const std::string& fallback = ""
    )
    {
        if (!item.contains(key) || item[key].is_null()) {
            return fallback;
        }

        if (item[key].is_string()) {
            return item[key].get<std::string>();
        }

        return fallback;
    }

    json normalizeAsset(
        const json& item
    )
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

    json normalizeAssetsResponse(
        const json& response
    )
    {
        json assets = json::array();
        json data = json::array();

        if (response.contains("data") && response["data"].is_array()) {
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
}

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
        return normalizeAssetsResponse(response);
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

    if (res.status_code != 200 && res.status_code != 204) {
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
        return normalizeAssetsResponse(response);
    }
    catch (const std::exception& ex) {
        std::cout << "Parse search assets failed: " << ex.what() << std::endl;
    }

    return json::array();
}
