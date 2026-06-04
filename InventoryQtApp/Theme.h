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

    static void applyApplication(QApplication& app);
    static void applyLogin(QWidget* widget);
    static void applyTheme(AppTheme theme);

    static QString globalStyleSheet();
    static QString darkStyleSheet();
    static QString lightStyleSheet();

    static QString loginStyle(AppTheme theme = AppTheme::Dark);

    static QString shellStyle(AppTheme theme);
    static QString sidebarStyle(AppTheme theme);
    static QString verticalBarStyle(AppTheme theme);

    static QString dashboardPageStyle(AppTheme theme);
    static QString settingsPageStyle(AppTheme theme);
    static QString scanPageStyle(AppTheme theme);

    static QString itemsPageStyle(AppTheme theme);
    static QString assetsPageStyle(AppTheme theme);
    static QString usersPageStyle(AppTheme theme);

    static QString truckStockDashboardPageStyle(AppTheme theme);
    static QString trucksPageStyle(AppTheme theme);
    static QString templatesPageStyle(AppTheme theme);
    static QString assignmentsPageStyle(AppTheme theme);
    static QString myTruckStockPageStyle(AppTheme theme);
    static QString lowStockAlertsPageStyle(AppTheme theme);
    static QString receiptsPageStyle(AppTheme theme);

    static QString dataPageStyle(AppTheme theme);
    static QString dialogStyle(AppTheme theme);
    static QString truckPageStyle(AppTheme theme);

    static QString reportsPageStyle(AppTheme theme);
};
