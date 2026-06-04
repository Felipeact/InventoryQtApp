#pragma once

#include <QString>
#include <QUrl>
#include <QObject>
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct UpdateInfo
{
    QString version;
    QString downloadUrl;
    QString releaseNotes;
    bool isRequired;
    QString changeLog;
};

// Forward declaration
class QNetworkAccessManager;
class QNetworkReply;

class AutoUpdateManager : public QObject
{
    Q_OBJECT

public:
    AutoUpdateManager(QObject* parent = nullptr);
    ~AutoUpdateManager();

    // Check for updates
    void checkForUpdates();

    // Download and install update
    void downloadAndInstallUpdate(const UpdateInfo& updateInfo);

    // Get current app version
    static QString getCurrentVersion();

    // Check if update is needed
    static bool isUpdateNeeded(const QString& latestVersion);

    // Get update info from remote server
    UpdateInfo parseUpdateInfo(const json& data);

signals:
    void updateAvailable(const UpdateInfo& info);
    void noUpdateAvailable();
    void updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updateDownloadFinished();
    void updateError(const QString& errorMessage);

private slots:
    void onReplyFinished();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QNetworkAccessManager* networkManager;
    QString updateCheckUrl;

    void compareVersions(const QString& latestVersion);
};
