// Tests for Config: resolution order of API base URL
// (environment variable > QSettings > built-in default) and trimming.
#include <QtTest>
#include <QStandardPaths>
#include <QSettings>

#include "Config.h"

class TestConfig : public QObject
{
    Q_OBJECT

private:
    void clearStoredSettings()
    {
        QSettings settings("InventorySystem", "InventoryQtApp");
        settings.clear();
        settings.sync();
    }

private slots:
    void initTestCase()
    {
        // Redirect QSettings to a throwaway location so the test never touches
        // (or depends on) real user configuration.
        QStandardPaths::setTestModeEnabled(true);
    }

    void init()
    {
        qunsetenv("INVENTORY_APP_API_URL");
        clearStoredSettings();
    }

    void defaultUrl_whenNothingConfigured()
    {
        QCOMPARE(Config::getApiBaseUrl(), Config::DEFAULT_API_BASE_URL);
    }

    void environmentVariable_takesPrecedence()
    {
        Config::setApiBaseUrl("https://from-settings");
        qputenv("INVENTORY_APP_API_URL", "https://from-env");

        QCOMPARE(Config::getApiBaseUrl(), std::string("https://from-env"));
    }

    void settings_usedWhenNoEnvironmentVariable()
    {
        Config::setApiBaseUrl("https://from-settings");
        QCOMPARE(Config::getApiBaseUrl(), std::string("https://from-settings"));
    }

    void settings_areTrimmed()
    {
        Config::setApiBaseUrl("   https://trimmed   ");
        QCOMPARE(Config::getApiBaseUrl(), std::string("https://trimmed"));
    }

    void reset_revertsToDefault()
    {
        Config::setApiBaseUrl("https://custom");
        QCOMPARE(Config::getApiBaseUrl(), std::string("https://custom"));

        Config::resetApiBaseUrl();
        QCOMPARE(Config::getApiBaseUrl(), Config::DEFAULT_API_BASE_URL);
    }

    void emptySetting_fallsBackToDefault()
    {
        Config::setApiBaseUrl("    ");
        QCOMPARE(Config::getApiBaseUrl(), Config::DEFAULT_API_BASE_URL);
    }
};

QTEST_GUILESS_MAIN(TestConfig)
#include "tst_config.moc"
