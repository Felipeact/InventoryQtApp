#include "ReportService.h"
#include "Logging.h"


ReportService::ReportService(ApiClient& apiClient) : api(apiClient)
{
}

json ReportService::getInventorySummary()
{
    auto res = api.get("/reports/inventory-summary");

    if (res.status_code != 200) {
        return json::object();
    }

    try {
        return json::parse(res.text);
    }
    catch (const std::exception& ex) {
        inv::logWarning(std::string("Parse inventory summary failed: ") + ex.what());
        return json::object();
    }
}

json ReportService::getAssetsSummary()
{
    auto res = api.get("/reports/assets-summary");

    if (res.status_code != 200) {
        return json::object();
    }

    try {
        return json::parse(res.text);
    }
    catch (const std::exception& ex) {
        inv::logWarning(std::string("Parse assets summary failed: ") + ex.what());
        return json::object();
    }
}
