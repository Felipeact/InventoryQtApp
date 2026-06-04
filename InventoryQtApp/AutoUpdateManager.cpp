#include "AutoUpdateManager.h"
#include <QSettings>
#include <QUrl>
#include <QFile>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

// QtNetwork includes - optional
#ifdef QT_NETWORK_LIB
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#endif

AutoUpdateManager::AutoUpdateManager(const QString& checkUrl, QObject* parent)
    : QObject(parent),
      updateCheckUrl(checkUrl)
{
#ifdef QT_NETWORK_LIB
    networkManager = new QNetworkAccessManager(this);
    if (networkManager) {
        // Connect network manager signals
        connect(networkManager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onNetworkReplyFinished(QNetworkReply*)));
    }
#endif
}

AutoUpdateManager::~AutoUpdateManager()
{
}

void AutoUpdateManager::onNetworkReplyFinished(QNetworkReply* reply)
{
#ifdef QT_NETWORK_LIB
    if (!reply) return;

    if (reply->property("downloadMode").toBool()) {
        onDownloadReplyFinished();
    } else {
        onCheckReplyFinished();
    }
    reply->deleteLater();
#endif
}

void AutoUpdateManager::checkForUpdates()
{
#ifdef QT_NETWORK_LIB
    if (!networkManager) {
        emit noUpdateAvailable();
        return;
    }

    qDebug() << "Checking for updates from:" << updateCheckUrl;

    QNetworkRequest request(QUrl(updateCheckUrl));
    request.setHeader(QNetworkRequest::UserAgentHeader, "InventoryQtApp/1.0");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = networkManager->get(request);
    if (!reply) {
        emit noUpdateAvailable();
        return;
    }

    reply->setProperty("downloadMode", false);
#else
    qWarning() << "QtNetwork not available. Update checking disabled.";
    emit noUpdateAvailable();
#endif
}

void AutoUpdateManager::downloadAndInstallUpdate(const UpdateInfo& updateInfo)
{
#ifdef QT_NETWORK_LIB
    if (!networkManager) return;

    // Save download path
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    currentDownloadPath = tempDir + "/InventoryQtApp-" + updateInfo.version + ".exe";

    qDebug() << "Downloading update from:" << updateInfo.downloadUrl;
    qDebug() << "Saving to:" << currentDownloadPath;

    QNetworkRequest request(QUrl(updateInfo.downloadUrl));
    request.setHeader(QNetworkRequest::UserAgentHeader, "InventoryQtApp/1.0");

    QNetworkReply* reply = networkManager->get(request);
    if (!reply) {
        emit updateError("Failed to start download");
        return;
    }

    reply->setProperty("downloadMode", true);

    connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(onDownloadProgress(qint64, qint64)));
#else
    qWarning() << "QtNetwork not available. Update download disabled.";
    emit updateError("QtNetwork module not available");
#endif
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

void AutoUpdateManager::onCheckReplyFinished()
{
#ifdef QT_NETWORK_LIB
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {
        try {
            QString responseStr = QString::fromUtf8(reply->readAll());
            qDebug() << "Update check response:" << responseStr;

            auto jsonData = json::parse(responseStr.toStdString());

            QString latestVersion = QString::fromStdString(
                jsonData["version"].get<std::string>()
            );

            qDebug() << "Latest version:" << latestVersion;
            qDebug() << "Current version:" << getCurrentVersion();

            if (isUpdateNeeded(latestVersion)) {
                UpdateInfo updateInfo = parseUpdateInfo(jsonData);
                qDebug() << "Update available:" << updateInfo.version;
                emit updateAvailable(updateInfo);
            } else {
                qDebug() << "No update needed";
                emit noUpdateAvailable();
            }
        }
        catch (const std::exception& e) {
            qWarning() << "Failed to parse update response:" << e.what();
            emit updateError("Failed to parse update response: " + QString::fromStdString(std::string(e.what())));
        }
    } else {
        qWarning() << "Update check error:" << reply->errorString();
        emit updateError("Network error: " + reply->errorString());
    }
#endif
}

void AutoUpdateManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
#ifdef QT_NETWORK_LIB
    qDebug() << "Download progress:" << bytesReceived << "/" << bytesTotal;
    emit updateDownloadProgress(bytesReceived, bytesTotal);
#endif
}

void AutoUpdateManager::onDownloadReplyFinished()
{
#ifdef QT_NETWORK_LIB
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {
        // Save downloaded file
        QFile file(currentDownloadPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();

            qDebug() << "Update downloaded to:" << currentDownloadPath;
            emit updateDownloadFinished(currentDownloadPath);
        } else {
            qWarning() << "Failed to save update file";
            emit updateError("Failed to save update file");
        }
    } else {
        qWarning() << "Download error:" << reply->errorString();
        emit updateError("Download error: " + reply->errorString());
    }
#endif
}

void AutoUpdateManager::compareVersions(const QString& latestVersion)
{
    // Implementation handled in onCheckReplyFinished()
}
