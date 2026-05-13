#pragma once

#include "ApiClient.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ReportService
{

private:
	ApiClient& api; // Reference to the API client for making requests

public:

	ReportService(ApiClient& apiClient);
	
	json getInventorySummary();
	json getAssetsSummary();


};