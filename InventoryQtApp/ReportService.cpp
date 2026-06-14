#include "ReportService.h"

namespace
{
    // Parses a response body without throwing; returns an empty object when the
    // payload is missing or malformed so callers always get a usable json value.
    json parseOrEmptyObject(const std::string& text)
    {
        json parsed = json::parse(text, nullptr, false);
        if (parsed.is_discarded()) {
            return json::object();
        }
        return parsed;
    }
}

ReportService::ReportService(IApiClient& apiClient) : api(apiClient)
{
}

json ReportService::getInventorySummary()
{
    auto res = api.get("/reports/inventory-summary");

    if (res.status_code != 200) {
        return json::object();
    }

    return parseOrEmptyObject(res.text);
}

json ReportService::getAssetsSummary()
{
    auto res = api.get("/reports/assets-summary");

    if (res.status_code != 200) {
        return json::object();
    }

    return parseOrEmptyObject(res.text);
}
