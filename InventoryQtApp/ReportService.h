#pragma once

#include "IApiClient.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ReportService
{

private:
	IApiClient& api; // Reference to the API client for making requests

public:

	ReportService(IApiClient& apiClient);
	
	json getInventorySummary();
	json getAssetsSummary();


};