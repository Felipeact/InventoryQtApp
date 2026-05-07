// AssetService.cpp - Implementation of asset service
#include "AssetService.h"
#include <iostream>

AssetService::AssetService(ApiClient& apiClient)
    : api(apiClient)
{
}

bool AssetService::createAsset(const std::string& name, const std::string& type, const std::string& serialCode, const std::string& status, const std::string& description)
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
        std::cout << res.text << std::endl;
        return false;
    }

    cachedAssets = json::array(); // clear cache after change
    return true;
}

json AssetService::getAssets(bool forceRefresh)
{
    if (!forceRefresh && !cachedAssets.empty()) {
        return cachedAssets;
    }

    auto res = api.get("/assets");

    if (res.status_code != 200) {
        std::cout << res.text << std::endl;
        return json::array();
    }

    cachedAssets = json::parse(res.text);
    return cachedAssets;
}

bool AssetService::updateAsset(const std::string& assetId, const std::string& name, const std::string& type, const std::string& serialCode, const std::string& status, const std::string& description)
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
        std::cout << res.text << std::endl;
        return false;
    }

    cachedAssets = json::array();
    return true;

}

bool AssetService::deleteAsset(const std::string& assetId)
{
    auto res = api.del("/assets/" + assetId);

    if (res.status_code != 200) {
        std::cout << res.text << std::endl;
        return false;
    }

    cachedAssets = json::array();
    return true;
}

