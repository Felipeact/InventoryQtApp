#include "AutoUpdateManager.h"

#include <QSettings>
#include <QUrl>
#include <QFile>
#include <QStandardPaths>
#include <QDebug>

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

    connect(networkManager, &QNetworkAccessManager::finished,
        this, &AutoUpdateManager::onNetworkReplyFinished);
#else
    qWarning() << "QtNetwork not available. Auto update disabled.";
#endif
}

AutoUpdateManager::~AutoUpdateManager()
{
}

void AutoUpdateManager::onNetworkReplyFinished(QNetworkReply* reply)
{
#ifdef QT_NETWORK_LIB
    if (!reply) {
        return;
    }

    const bool isDownload = reply->property("downloadMode").toBool();

    if (isDownload) {
        onDownloadReplyFinished(reply);
    }
    else {
        onCheckReplyFinished(reply);
    }

    reply->deleteLater();
#else
    Q_UNUSED(reply);
#endif
}

void AutoUpdateManager::checkForUpdates()
{
#ifdef QT_NETWORK_LIB
    if (!networkManager) {
        emit noUpdateAvailable();
        return;
    }

    if (updateCheckUrl.trimmed().isEmpty()) {
        emit updateError("Update check URL is empty.");
        return;
    }

    qDebug() << "Checking for updates from:" << updateCheckUrl;

    QUrl url(updateCheckUrl);
    QNetworkRequest request(url);

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
    if (!networkManager) {
        emit updateError("Network manager is not available.");
        return;
    }

    if (updateInfo.downloadUrl.trimmed().isEmpty()) {
        emit updateError("Download URL is empty.");
        return;
    }

    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    currentDownloadPath = tempDir + "/InventoryQtApp-" + updateInfo.version + ".exe";

    qDebug() << "Downloading update from:" << updateInfo.downloadUrl;
    qDebug() << "Saving to:" << currentDownloadPath;

    QUrl url(updateInfo.downloadUrl);
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::UserAgentHeader, "InventoryQtApp/1.0");

    QNetworkReply* reply = networkManager->get(request);

    if (!reply) {
        emit updateError("Failed to start download.");
        return;
    }

    reply->setProperty("downloadMode", true);

    connect(reply, &QNetworkReply::downloadProgress,
        this, &AutoUpdateManager::onDownloadProgress);
#else
    qWarning() << "QtNetwork not available. Update download disabled.";
    emit updateError("QtNetwork module not available.");
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

    QStringList currentParts = currentVersion.split(".");
    QStringList latestParts = latestVersion.split(".");

    int maxParts = qMax(currentParts.size(), latestParts.size());

    for (int i = 0; i < maxParts; ++i) {
        int current = i < currentParts.size() ? currentParts[i].toInt() : 0;
        int latest = i < latestParts.size() ? latestParts[i].toInt() : 0;

        if (latest > current) {
            return true;
        }

        if (latest < current) {
            return false;
        }
    }

    return false;
}

UpdateInfo AutoUpdateManager::parseUpdateInfo(const json& data)
{
    UpdateInfo info;

    try {
        if (data.contains("version")) {
            info.version = QString::fromStdString(data["version"].get<std::string>());
        }

        if (data.contains("downloadUrl")) {
            info.downloadUrl = QString::fromStdString(data["downloadUrl"].get<std::string>());
        }
        else if (data.contains("url")) {
            // Supports backend response using "url"
            info.downloadUrl = QString::fromStdString(data["url"].get<std::string>());
        }

        if (data.contains("releaseNotes")) {
            info.releaseNotes = QString::fromStdString(data["releaseNotes"].get<std::string>());
        }

        if (data.contains("required")) {
            info.isRequired = data["required"].get<bool>();
        }

        if (data.contains("changeLog")) {
            info.changeLog = QString::fromStdString(data["changeLog"].get<std::string>());
        }
    }
    catch (const std::exception& e) {
        qWarning() << "Failed to parse update info:" << e.what();
    }

    return info;
}

void AutoUpdateManager::onCheckReplyFinished(QNetworkReply* reply)
{
#ifdef QT_NETWORK_LIB
    if (!reply) {
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Update check error:" << reply->errorString();
        emit updateError("Network error: " + reply->errorString());
        return;
    }

    try {
        QString responseStr = QString::fromUtf8(reply->readAll());
        qDebug() << "Update check response:" << responseStr;

        auto jsonData = json::parse(responseStr.toStdString());

        if (!jsonData.contains("version")) {
            emit updateError("Update response missing version.");
            return;
        }

        QString latestVersion = QString::fromStdString(
            jsonData["version"].get<std::string>()
        );

        qDebug() << "Latest version:" << latestVersion;
        qDebug() << "Current version:" << getCurrentVersion();

        if (isUpdateNeeded(latestVersion)) {
            UpdateInfo updateInfo = parseUpdateInfo(jsonData);

            if (updateInfo.downloadUrl.trimmed().isEmpty()) {
                emit updateError("Update available, but download URL is missing.");
                return;
            }

            qDebug() << "Update available:" << updateInfo.version;
            emit updateAvailable(updateInfo);
        }
        else {
            qDebug() << "No update needed";
            emit noUpdateAvailable();
        }
    }
    catch (const std::exception& e) {
        qWarning() << "Failed to parse update response:" << e.what();
        emit updateError("Failed to parse update response: " + QString::fromStdString(e.what()));
    }
#else
    Q_UNUSED(reply);
#endif
}

void AutoUpdateManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
#ifdef QT_NETWORK_LIB
    qDebug() << "Download progress:" << bytesReceived << "/" << bytesTotal;
    emit updateDownloadProgress(bytesReceived, bytesTotal);
#else
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);
#endif
}

void AutoUpdateManager::onDownloadReplyFinished(QNetworkReply* reply)
{
#ifdef QT_NETWORK_LIB
    if (!reply) {
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Download error:" << reply->errorString();
        emit updateError("Download error: " + reply->errorString());
        return;
    }

    QFile file(currentDownloadPath);

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save update file:" << currentDownloadPath;
        emit updateError("Failed to save update file.");
        return;
    }

    file.write(reply->readAll());
    file.close();

    qDebug() << "Update downloaded to:" << currentDownloadPath;
    emit updateDownloadFinished(currentDownloadPath);
#else
    Q_UNUSED(reply);
#endif
}

void AutoUpdateManager::compareVersions(const QString& latestVersion)
{
    Q_UNUSED(latestVersion);
}