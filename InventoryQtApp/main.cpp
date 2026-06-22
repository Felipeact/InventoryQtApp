// main.cpp - Entry point for the Inventory Management System application
#include "InventoryQtApp.h"
#include "Version.h"

#include <QtWidgets/QApplication>
#include <QIcon>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QtGlobal>

#include <cstdio>
#include <exception>

namespace
{
    QFile g_logFile;
    QMutex g_logMutex;

    // Per-user log location, e.g. %APPDATA%/InventorySystem/InventoryQtApp on Windows.
    QString logFilePath()
    {
        const QString dir =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(dir);
        return dir + "/inventory-app.log";
    }

    // Routes all qDebug/qInfo/qWarning/qCritical output to a rolling log file so that
    // a GUI-subsystem build (which has no console) still produces diagnostics in the
    // field. Falls back to stderr as well.
    void messageHandler(QtMsgType type,
                        const QMessageLogContext& context,
                        const QString& msg)
    {
        Q_UNUSED(context);

        const char* level = "INFO";
        switch (type) {
        case QtDebugMsg:    level = "DEBUG"; break;
        case QtInfoMsg:     level = "INFO";  break;
        case QtWarningMsg:  level = "WARN";  break;
        case QtCriticalMsg: level = "ERROR"; break;
        case QtFatalMsg:    level = "FATAL"; break;
        }

        const QString line =
            QStringLiteral("%1 [%2] %3")
                .arg(QDateTime::currentDateTime().toString(Qt::ISODate),
                     QString::fromLatin1(level),
                     msg);

        QMutexLocker locker(&g_logMutex);
        if (g_logFile.isOpen()) {
            QTextStream out(&g_logFile);
            out << line << '\n';
            out.flush();
        }
        std::fprintf(stderr, "%s\n", line.toLocal8Bit().constData());

        if (type == QtFatalMsg) {
            std::fflush(stderr);
            abort();
        }
    }

    void installFileLogger()
    {
        const QString path = logFilePath();

        // Simple size-based rotation: start a fresh log once it passes ~5 MB.
        if (QFileInfo(path).size() > 5 * 1024 * 1024) {
            QFile::remove(path);
        }

        g_logFile.setFileName(path);
        g_logFile.open(QIODevice::Append | QIODevice::Text);
        qInstallMessageHandler(messageHandler);
    }
}

// Main function initializes and runs the application
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Stable application identity so QSettings, update logic and OS integration are
    // consistent regardless of where they are constructed.
    QApplication::setOrganizationName(QStringLiteral("InventorySystem"));
    QApplication::setApplicationName(QStringLiteral("InventoryQtApp"));
    QApplication::setApplicationVersion(QStringLiteral(INVENTORY_APP_VERSION));

    // Brand icon (same mark as the web and mobile apps), used for the window title
    // bar and taskbar. The Windows .exe icon comes from app_icon.rc at link time.
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/InventoryQtApp/app_icon.png")));

    installFileLogger();

    // Publish the compile-time version into the QSettings key the auto-updater reads,
    // so update checks compare against the real running version (not a hardcoded 1.0.0).
    {
        QSettings settings(QStringLiteral("InventorySystem"),
                           QStringLiteral("InventoryQtApp"));
        settings.setValue(QStringLiteral("app/version"),
                          QStringLiteral(INVENTORY_APP_VERSION));
    }

    try {
        InventoryQtApp window;
        window.show();
        return app.exec();
    }
    catch (const std::exception& ex) {
        qCritical() << "Fatal unhandled exception:" << ex.what();
        return 1;
    }
    catch (...) {
        qCritical() << "Fatal unknown exception";
        return 1;
    }
}
