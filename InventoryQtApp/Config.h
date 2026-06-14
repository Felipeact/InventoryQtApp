#pragma once

#include <string>
#include <cstdlib>
#include <cstring>

#include <QString>
#include <QSettings>

#ifdef _WIN32
#include <stdlib.h>
#endif

// Compiled-in default endpoints. These are intentionally empty so that no
// throwaway development URL is ever baked into a shipped binary. Production
// builds should supply real values at build time, e.g.:
//
//   -DINVENTORY_DEFAULT_API_URL="https://api.yourcompany.com"
//   -DINVENTORY_DEFAULT_UPDATE_URL="https://api.yourcompany.com/updates/latest"
//
// At runtime the value is still overridable via the INVENTORY_APP_API_URL /
// INVENTORY_APP_UPDATE_URL environment variables or the in-app Settings page.
// When left empty the app surfaces a connection error until configured.
#ifndef INVENTORY_DEFAULT_API_URL
#define INVENTORY_DEFAULT_API_URL ""
#endif

#ifndef INVENTORY_DEFAULT_UPDATE_URL
#define INVENTORY_DEFAULT_UPDATE_URL ""
#endif

namespace Config
{
    const std::string DEFAULT_API_BASE_URL = INVENTORY_DEFAULT_API_URL;

    const std::string DEFAULT_UPDATE_SERVER_URL = INVENTORY_DEFAULT_UPDATE_URL;

    inline std::string getEnvironmentValue(
        const char* key
    )
    {
#ifdef _WIN32
        char* envValue = nullptr;
        size_t envSize = 0;

        _dupenv_s(
            &envValue,
            &envSize,
            key
        );

        if (envValue && strlen(envValue) > 0) {
            std::string result(envValue);
            free(envValue);
            return result;
        }

        if (envValue) {
            free(envValue);
        }

        return "";
#else
        const char* envValue =
            std::getenv(key);

        if (envValue && strlen(envValue) > 0) {
            return std::string(envValue);
        }

        return "";
#endif
    }

    inline std::string getApiBaseUrl()
    {
        std::string envUrl =
            getEnvironmentValue("INVENTORY_APP_API_URL");

        if (!envUrl.empty()) {
            return envUrl;
        }

        QSettings settings(
            "InventorySystem",
            "InventoryQtApp"
        );

        QString settingsUrl =
            settings.value(
                "api/baseUrl",
                QString::fromStdString(DEFAULT_API_BASE_URL)
            ).toString();

        if (!settingsUrl.trimmed().isEmpty()) {
            return settingsUrl.trimmed().toStdString();
        }

        return DEFAULT_API_BASE_URL;
    }

    inline std::string getUpdateServerUrl()
    {
        std::string envUrl =
            getEnvironmentValue("INVENTORY_APP_UPDATE_URL");

        if (!envUrl.empty()) {
            return envUrl;
        }

        QSettings settings(
            "InventorySystem",
            "InventoryQtApp"
        );

        QString settingsUrl =
            settings.value(
                "updates/checkUrl",
                QString::fromStdString(DEFAULT_UPDATE_SERVER_URL)
            ).toString();

        return settingsUrl.trimmed().toStdString();
    }

    inline void setApiBaseUrl(
        const std::string& url
    )
    {
        QSettings settings(
            "InventorySystem",
            "InventoryQtApp"
        );

        settings.setValue(
            "api/baseUrl",
            QString::fromStdString(url)
        );
    }

    inline void resetApiBaseUrl()
    {
        QSettings settings(
            "InventorySystem",
            "InventoryQtApp"
        );

        settings.remove("api/baseUrl");
    }

    inline std::string API_BASE_URL()
    {
        return getApiBaseUrl();
    }
}