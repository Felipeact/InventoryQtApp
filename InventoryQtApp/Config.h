#pragma once

#include <string>
#include <cstdlib>
#include <cstring>
#include <QSettings>

#ifdef _WIN32
#include <stdlib.h>
#endif

namespace Config
{
	// Default API URL (can be overridden by settings or environment variable)
	// TODO: Update this to your production API URL before deployment
	// Example: "https://api.your-company.com" or "https://api.your-domain.com"
	const std::string DEFAULT_API_BASE_URL = "https://orange-robot-5x4pq4vr5vw3p9g-3000.app.github.dev";

	// Update server URL for auto-updates
	// TODO: Configure your update server URL before deployment
	const std::string UPDATE_SERVER_URL = "/updates/latest";

	// Get API URL from settings or environment
	inline std::string getApiBaseUrl()
	{
		// Try environment variable first
#ifdef _WIN32
		char* envUrl = nullptr;
		size_t envSize = 0;
		_dupenv_s(&envUrl, &envSize, "INVENTORY_APP_API_URL");
		if (envUrl && strlen(envUrl) > 0) {
			std::string result(envUrl);
			free(envUrl);
			return result;
		}
#else
		const char* envUrl = std::getenv("INVENTORY_APP_API_URL");
		if (envUrl && strlen(envUrl) > 0) {
			return std::string(envUrl);
		}
#endif

		// Try QSettings (stored in registry/ini file)
		QSettings settings("InventorySystem", "InventoryQtApp");
		QString settingsUrl = settings.value("api/baseUrl", "").toString();

		if (!settingsUrl.isEmpty()) {
			return settingsUrl.toStdString();
		}

		// Fall back to default
		return DEFAULT_API_BASE_URL;
	}

	// Set API URL in settings
	inline void setApiBaseUrl(const std::string& url)
	{
		QSettings settings("InventorySystem", "InventoryQtApp");
		settings.setValue("api/baseUrl", QString::fromStdString(url));
	}

	// Get the API URL (must be called after QSettings is available)
	inline std::string API_BASE_URL()
	{
		return getApiBaseUrl();
	}
}
