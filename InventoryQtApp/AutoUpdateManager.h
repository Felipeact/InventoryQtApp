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
    bool isRequired = false;
    QString changeLog;
};

class QNetworkAccessManager;
class QNetworkReply;

class AutoUpdateManager : public QObject
{
    Q_OBJECT

public:
    explicit AutoUpdateManager(const QString& checkUrl, QObject* parent = nullptr);
    ~AutoUpdateManager();

    void checkForUpdates();
    void downloadAndInstallUpdate(const UpdateInfo& updateInfo);

    static QString getCurrentVersion();
    static bool isUpdateNeeded(const QString& latestVersion);

    UpdateInfo parseUpdateInfo(const json& data);

signals:
    void updateAvailable(const UpdateInfo& info);
    void noUpdateAvailable();
    void updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updateDownloadFinished(const QString& filePath);
    void updateError(const QString& errorMessage);

private slots:
    void onNetworkReplyFinished(QNetworkReply* reply);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QNetworkAccessManager* networkManager = nullptr;
    QString updateCheckUrl;
    QString currentDownloadPath;

    void onCheckReplyFinished(QNetworkReply* reply);
    void onDownloadReplyFinished(QNetworkReply* reply);
    void compareVersions(const QString& latestVersion);
};