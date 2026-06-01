#pragma once

#include <QWidget>
#include <QString>
#include <string>

#include "ui_SettingsPage.h"
#include "Theme.h"

class SettingsPage : public QWidget
{
    Q_OBJECT

public:

    void applyTheme(Theme::AppTheme theme);
    explicit SettingsPage(
        const std::string& role,
        const std::string& userName,
        QWidget* parent = nullptr
    );

    ~SettingsPage();

    void setUserInfo(
        const std::string& role,
        const std::string& userName
    );

signals:
    void userNameChanged(const std::string& newUserName);
    void themeChanged(const QString& themeName);
    void logoutRequested();

private slots:
    void onSaveProfileClicked();
    void onApplyThemeClicked();
    void onLogoutClicked();

private:
    Ui::SettingsPageClass ui;

    std::string role;
    std::string userName;
};