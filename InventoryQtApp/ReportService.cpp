#include "ReportService.h"


ReportService::ReportService(ApiClient& apiClient) : api(apiClient)
{
}

json ReportService::getInventorySummary()
{
    auto res = api.get("/reports/inventory-summary");

    if (res.status_code != 200) {
        return json::object();
    }

    return json::parse(res.text);
}

json ReportService::getAssetsSummary()
{
    auto res = api.get("/reports/assets-summary");

    if (res.status_code != 200) {
        return json::object();
    }

    return json::parse(res.text);
}