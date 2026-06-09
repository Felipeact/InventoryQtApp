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

    const std::string DEFAULT_UPDATE_SERVER_URL =
        "https://orange-robot-5x4pq4vr5vw3p9g-3000.app.github.dev/updates/latest";

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