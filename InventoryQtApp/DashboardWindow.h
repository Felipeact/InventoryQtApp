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
#include <FleetPage.h>
#include <MyTruckStockPage.h>
#include <LowStockAlertsPage.h>
#include <ReceiptsPage.h>
#include <TruckCostsPage.h>
#include <ReportsPage.h>
#include <TruckStockService.h>
#include <SettingsPage.h>
#include "AiService.h"
#include "AiAssistantPage.h"

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
        AiService& aiService,
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
    AiService* aiService = nullptr;

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
    FleetPage* fleetPage = nullptr;
    MyTruckStockPage* myTruckStockPage = nullptr;
    LowStockAlertsPage* lowStockAlertsPage = nullptr;
    ReceiptsPage* receiptsPage = nullptr;
    TruckCostsPage* truckCostsPage = nullptr;
    ReportsPage* reportsPage = nullptr;
    AiAssistantPage* aiAssistantPage = nullptr;

    Theme::AppTheme currentTheme = Theme::AppTheme::Light;

    void setupSidebar();
    void setupVerticalbar();
    void setupPages();
    void loadSavedTheme();
    void applyThemeToLoadedPages();

    DashboardPage* ensureDashboardPage();
    ItemsPage* ensureItemsPage();
    AssetsPage* ensureAssetsPage();
    UsersPage* ensureUsersPage();
    ScanPage* ensureScanInPage();
    ScanPage* ensureScanOutPage();
    SettingsPage* ensureSettingsPage();
    TruckStockDashboardPage* ensureTruckStockDashboardPage();
    TrucksPage* ensureTrucksPage();
    StockTemplatesPage* ensureStockTemplatesPage();
    AssignmentsPage* ensureAssignmentsPage();
    FleetPage* ensureFleetPage();
    MyTruckStockPage* ensureMyTruckStockPage();
    LowStockAlertsPage* ensureLowStockAlertsPage();
    ReceiptsPage* ensureReceiptsPage();
    TruckCostsPage* ensureTruckCostsPage();
    ReportsPage* ensureReportsPage();
    AiAssistantPage* ensureAiAssistantPage();

    void showGlobalSearchDialog(const QString& text);
    void closeGlobalSearchDialog();
    void handleGlobalSearchResult(
        GlobalSearchDialog::SearchTarget target,
        const QString& value
    );

    /** Refresh the bell badge with the current low-stock + pending-receipt count. */
    void refreshNotificationBadge();

private slots:
    void onGlobalSearchTextChanged(const QString& text);
    void onNotificationRequested();
};
