#pragma once

#include <QString>
#include <QWidget>
#include <QApplication>

class Theme
{
public:
    enum class AppTheme
    {
        Dark,
        Light
    };

    static void applyApplication();
    static void applyApplication(QApplication& app);

    static void applyLogin(QWidget* widget);
    static void applyTheme(AppTheme theme);

    static QString globalStyleSheet();
    static QString darkStyleSheet();
    static QString lightStyleSheet();

    static QString sidebarStyle(AppTheme theme);
    static QString verticalBarStyle(AppTheme theme);
    static QString settingsPageStyle(AppTheme theme);
    static QString scanPageStyle(AppTheme theme);
    static QString dashboardPageStyle(AppTheme theme);

    static QString dataPageStyle(AppTheme theme);
    static QString dialogStyle(AppTheme theme);
    static QString truckPageStyle(AppTheme theme);
};
