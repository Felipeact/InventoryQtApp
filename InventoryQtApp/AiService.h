#pragma once

#include "ApiClient.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Talks to the backend AI assistant (/ai). The assistant can answer questions,
// build reports, and take actions across the company's data; all calls run with
// the signed-in user's own token, so the server enforces tenant + permission scope.
class AiService
{
private:
	ApiClient& api; // Reference to the API client for making requests

public:
	AiService(ApiClient& apiClient);

	// Sends the full conversation (a JSON array of { role, content } turns; the
	// last one is the new user message) and returns the parsed response
	// { reply, actions }. On failure returns { error: "<message>" } instead of
	// throwing, so the UI can show it inline.
	json chat(const json& messages);

	// Whether the assistant is configured on the server (ANTHROPIC_API_KEY set).
	bool isEnabled();
};
