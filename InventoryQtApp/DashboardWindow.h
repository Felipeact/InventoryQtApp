// DashboardWindow.h - Main application window after user login
#pragma once

#include <DashboardPage.h>

#include "NotificationDialog.h"
#include "GlobalSearchDialog.h"
#include "ProductService.h"
#include "AssetService.h"
#include "ReportService.h"

#include <AssetsPage.h>
#include <ItemsPage.h>
#include <ScanPage.h>
#include <TruckStockDashboardPage.h>
#include <TrucksPage.h>
#include <StockTemplatesPage.h>
#include <AssignmentsPage.h>
#include <MyTruckStockPage.h>
#include <LowStockAlertsPage.h>
#include <ReceiptsPage.h>
#include <ReportsPage.h>
#include <TruckStockService.h>
#include <SettingsPage.h>

#include <QMainWindow>
#include <QString>
#include <vector>
#include <string>

#include "ui_DashboardWindow.h"
#include "SidebarWidget.h"
#include "VerticalWidget.h"
#include <UsersPage.h>
#include "Theme.h"

class DashboardWindow : public QMainWindow
{
    Q_OBJECT

public:
    void applyTheme(Theme::AppTheme theme);

    DashboardWindow(
        const std::string& role,
        const std::vector<std::string>& permissions,
        const std::string& userName,
        ProductService& productService,
        AssetService& assetService,
        UserService& userService,
        ReportService& reportService,
        TruckStockService& truckStockService,
        QWidget* parent = nullptr
    );

    ~DashboardWindow();

    void updateLoggedInUserInfo(
        const std::string& newUserName
    );

signals:
    void logoutRequested();

private:
    Ui::DashboardWindowClass ui;

    std::string role;
    std::vector<std::string> permissions;
    std::string userName;

    SidebarWidget* sidebar = nullptr;
    VerticalWidget* verticalbar = nullptr;

    GlobalSearchDialog* globalSearchDialog = nullptr;

    ProductService* productService = nullptr;
    AssetService* assetService = nullptr;
    UserService* userService = nullptr;
    ReportService* reportService = nullptr;
    TruckStockService* truckStockService = nullptr;

    DashboardPage* dashboardPage = nullptr;
    ItemsPage* itemsPage = nullptr;
    AssetsPage* assetsPage = nullptr;
    UsersPage* usersPage = nullptr;

    ScanPage* scanInPage = nullptr;
    ScanPage* scanOutPage = nullptr;
    SettingsPage* settingsPage = nullptr;

    TruckStockDashboardPage* truckStockDashboardPage = nullptr;
    TrucksPage* trucksPage = nullptr;
    StockTemplatesPage* stockTemplatesPage = nullptr;
    AssignmentsPage* assignmentsPage = nullptr;
    MyTruckStockPage* myTruckStockPage = nullptr;
    LowStockAlertsPage* lowStockAlertsPage = nullptr;
    ReceiptsPage* receiptsPage = nullptr;
    ReportsPage* reportsPage = nullptr;

    void setupSidebar();
    void setupVerticalbar();
    void setupPages();

    void showGlobalSearchDialog(const QString& text);
    void closeGlobalSearchDialog();
    void handleGlobalSearchResult(
        GlobalSearchDialog::SearchTarget target,
        const QString& value
    );

private slots:
    void onGlobalSearchTextChanged(const QString& text);
    void onNotificationRequested();
};