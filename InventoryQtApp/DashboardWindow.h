// DashboardWindow.h - Main application window after user login
#pragma once
#include <DashboardPage.h>
#include <ItemsPage.h>
#include "ProductService.h"
#include "AssetService.h"
#include "ReportService.h"
#include <AssetsPage.h>
#include <ScanPage.h>
#include <TruckStockDashboardPage.h>
#include <TrucksPage.h>
#include <StockTemplatesPage.h>
#include <AssignmentsPage.h>
#include <MyTruckStockPage.h>
#include <LowStockAlertsPage.h>
#include <ReceiptsPage.h>
#include <TruckStockService.h>
#include <SettingsPage.h>

#include <QMainWindow>
#include <vector>
#include <string>

#include "ui_DashboardWindow.h"
#include "SidebarWidget.h"
#include "VerticalWidget.h"
#include <UsersPage.h>
#include "Theme.h"

// Main dashboard window that manages navigation and displays different pages
class DashboardWindow : public QMainWindow
{
	Q_OBJECT

public:

	void applyTheme(Theme::AppTheme theme);
	// Constructor that initializes the dashboard with user role and permissions
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

	// Destructor
	~DashboardWindow();

	void updateLoggedInUserInfo(const std::string& newUserName);

signals:
	void logoutRequested();

private:
	Ui::DashboardWindowClass ui;           // UI components

	std::string role;                      // User's role
	std::vector<std::string> permissions;  // User's permissions
	std::string userName;

	SidebarWidget* sidebar = nullptr;           // Left navigation sidebar
	VerticalWidget* verticalbar = nullptr;        // Top user info bar

	ProductService* productService = nullptr; // Product service for API interactions
	AssetService* assetService = nullptr;   // Assets service for API interactions
	UserService* userService = nullptr;   // User service for API interactions
	ReportService* reportService = nullptr; // Report service for API interactions
	TruckStockService* truckStockService = nullptr;



	// Pages displayed in the main stack
	DashboardPage* dashboardPage = nullptr;   // Dashboard overview page
	ItemsPage* itemsPage = nullptr;           // Items management page
	AssetsPage* assetsPage = nullptr;         // Assets management page
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


	// Initialization methods
	void setupSidebar();       // Configures the sidebar navigation
	void setupVerticalbar();     // Configures the top user information bar
	void setupPages(); // Initializes the dashboard page


};