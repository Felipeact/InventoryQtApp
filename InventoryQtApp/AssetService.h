// AssetService.h - Service for asset operations
#pragma once
#include "ApiClient.h"
#include <nlohmann/json.hpp>
#include <string>
using json = nlohmann::json;


// Service class for handling asset-related operations
class AssetService
{
private:
    ApiClient& api;
    json cachedAssets = json::array();

public:
    AssetService(ApiClient& apiClient);

    bool createAsset(const std::string& name, const std::string& type, const std::string& serialCode, const std::string& status, const std::string& description = "");
    json getAssets(bool forceRefresh = false);

    bool updateAsset(const std::string& assetId, const std::string& name, const std::string& type, const std::string& serialCode, const std::string& status, const std::string& description = "");

    bool deleteAsset(const std::string& assetId);

    json searchAssets(const std::string& searchText);
};

