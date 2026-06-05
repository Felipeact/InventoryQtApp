#pragma once

#include <string>
#include <cstdlib>
#include <cstring>

#include <QString>
#include <QSettings>

#ifdef _WIN32
#include <stdlib.h>
#endif

namespace Config
{
    const std::string DEFAULT_API_BASE_URL =
        "https://orange-robot-5x4pq4vr5vw3p9g-3000.app.github.dev";

    const std::string UPDATE_SERVER_URL =
        "/updates/latest";

    inline std::string getApiBaseUrl()
    {
#ifdef _WIN32
        char* envUrl = nullptr;
        size_t envSize = 0;

        _dupenv_s(
            &envUrl,
            &envSize,
            "INVENTORY_APP_API_URL"
        );

        if (envUrl && strlen(envUrl) > 0) {
            std::string result(envUrl);
            free(envUrl);
            return result;
        }

        if (envUrl) {
            free(envUrl);
        }
#else
        const char* envUrl =
            std::getenv("INVENTORY_APP_API_URL");

        if (envUrl && strlen(envUrl) > 0) {
            return std::string(envUrl);
        }
#endif

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