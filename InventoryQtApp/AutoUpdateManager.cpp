#include "AutoUpdateManager.h"
#include <QSettings>

AutoUpdateManager::AutoUpdateManager(QObject* parent)
    : QObject(parent), networkManager(nullptr)
{
    updateCheckUrl = "https://api.your-update-server.com/latest-version";

    // Network manager initialization deferred to avoid requiring QtNetwork
    // in project configuration if not already present
}

AutoUpdateManager::~AutoUpdateManager()
{
}

void AutoUpdateManager::checkForUpdates()
{
    // Placeholder for update checking
    // In a real implementation with QtNetwork available:
    // QNetworkRequest request(QUrl(updateCheckUrl));
    // request.setHeader(QNetworkRequest::UserAgentHeader, "InventoryQtApp/1.0");
    // QNetworkReply* reply = networkManager->get(request);

    emit noUpdateAvailable();
}

void AutoUpdateManager::downloadAndInstallUpdate(const UpdateInfo& updateInfo)
{
    // Placeholder for update downloading
}

QString AutoUpdateManager::getCurrentVersion()
{
    QSettings settings("InventorySystem", "InventoryQtApp");
    return settings.value("app/version", "1.0.0").toString();
}

bool AutoUpdateManager::isUpdateNeeded(const QString& latestVersion)
{
    QString currentVersion = getCurrentVersion();

    // Parse versions (e.g., "1.2.3")
    QStringList currentParts = currentVersion.split(".");
    QStringList latestParts = latestVersion.split(".");

    // Compare major.minor.patch
    for (int i = 0; i < qMin(currentParts.size(), latestParts.size()); ++i) {
        int current = currentParts[i].toInt();
        int latest = latestParts[i].toInt();

        if (latest > current) {
            return true;  // Update available
        } else if (latest < current) {
            return false;  // Current version is newer
        }
    }

    // If we're here, versions are equal up to the common length
    return latestParts.size() > currentParts.size();
}

UpdateInfo AutoUpdateManager::parseUpdateInfo(const json& data)
{
    UpdateInfo info;

    try {
        info.version = QString::fromStdString(data["version"].get<std::string>());
        info.downloadUrl = QString::fromStdString(data["downloadUrl"].get<std::string>());
        info.releaseNotes = QString::fromStdString(
            data.contains("releaseNotes") ? data["releaseNotes"].get<std::string>() : ""
        );
        info.isRequired = data.contains("required") ? data["required"].get<bool>() : false;
        info.changeLog = QString::fromStdString(
            data.contains("changeLog") ? data["changeLog"].get<std::string>() : ""
        );
    }
    catch (const std::exception& e) {
        qWarning() << "Failed to parse update info:" << e.what();
    }

    return info;
}

void AutoUpdateManager::onReplyFinished()
{
    // Placeholder
}

void AutoUpdateManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit updateDownloadProgress(bytesReceived, bytesTotal);
}

void AutoUpdateManager::compareVersions(const QString& latestVersion)
{
    // Placeholder
}
