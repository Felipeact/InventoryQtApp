#pragma once

#include <QWidget>
#include <QString>
#include <string>

#include "ui_SettingsPage.h"
#include "Theme.h"
#include "UserService.h"

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(
        const std::string& role,
        const std::string& userName,
        UserService* userService,
        QWidget* parent = nullptr
    );

    ~SettingsPage();

    void applyTheme(
        Theme::AppTheme theme
    );

    void setUserInfo(
        const std::string& role,
        const std::string& userName
    );

signals:
    void userNameChanged(
        const std::string& newUserName
    );

    void themeChanged(
        const QString& themeName
    );

    void logoutRequested();

private slots:
    void onSaveProfileClicked();
    void onApplyThemeClicked();
    void onSaveApiUrlClicked();
    void onResetApiUrlClicked();
    void onLogoutClicked();

private:
    Ui::SettingsPageClass ui;

    std::string role;
    std::string userName;

    UserService* userService = nullptr;

    void loadSettings();

    bool isValidApiUrl(
        const QString& url
    ) const;
};