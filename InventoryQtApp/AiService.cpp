#include "AiService.h"
#include "Logging.h"

AiService::AiService(ApiClient& apiClient) : api(apiClient)
{
}

json AiService::chat(const json& messages)
{
    json body = { { "messages", messages } };

    auto res = api.post("/ai/chat", body.dump());

    if (res.status_code != 200) {
        std::string message = "The assistant request failed.";

        if (res.status_code == 0) {
            message = "Couldn't reach the server. Check your connection and try again.";
        }
        else {
            // Surface the API's error message when present (e.g. the 503 returned
            // when the assistant isn't configured server-side).
            try {
                json err = json::parse(res.text);
                if (err.contains("message") && err["message"].is_string()) {
                    message = err["message"].get<std::string>();
                }
                else if (err.contains("error") && err["error"].is_string()) {
                    message = err["error"].get<std::string>();
                }
            }
            catch (const std::exception&) {
                // Non-JSON error body — keep the generic message.
            }
        }

        return json{ { "error", message } };
    }

    try {
        return json::parse(res.text);
    }
    catch (const std::exception& ex) {
        inv::logWarning(std::string("Parse AI chat response failed: ") + ex.what());
        return json{ { "error", "Unexpected response from the assistant." } };
    }
}

bool AiService::isEnabled()
{
    auto res = api.get("/ai/status");

    if (res.status_code != 200) {
        return false;
    }

    try {
        json j = json::parse(res.text);
        return j.value("enabled", false);
    }
    catch (const std::exception&) {
        return false;
    }
}
