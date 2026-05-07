// DashboardWindow.h - Main application window after user login
#pragma once
#include <DashboardPage.h>
#include <ItemsPage.h>
#include "ProductService.h"
#include "AssetService.h"
#include <AssetsPage.h>

#include <QMainWindow>
#include <vector>
#include <string>

#include "ui_DashboardWindow.h"
#include "SidebarWidget.h"
#include "VerticalWidget.h"

// Main dashboard window that manages navigation and displays different pages
class DashboardWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor that initializes the dashboard with user role and permissions
    DashboardWindow(
        const std::string& role,
        const std::vector<std::string>& permissions,
		ProductService& productService,
		AssetService& assetService,
        QWidget* parent = nullptr
    );

    // Destructor
    ~DashboardWindow();

private:
	Ui::DashboardWindowClass ui;           // UI components

	std::string role;                      // User's role
	std::vector<std::string> permissions;  // User's permissions

	SidebarWidget* sidebar = nullptr;           // Left navigation sidebar
	VerticalWidget* verticalbar = nullptr;        // Top user info bar

	ProductService* productService = nullptr; // Product service for API interactions
	AssetService* assetService = nullptr;   // Assets service for API interactions

	// Pages displayed in the main stack
	DashboardPage* dashboardPage = nullptr;   // Dashboard overview page
	ItemsPage* itemsPage = nullptr;           // Items management page
	AssetsPage* assetsPage = nullptr;         // Assets management page
	

	// Initialization methods
	void setupSidebar();       // Configures the sidebar navigation
	void setupVerticalbar();     // Configures the top user information bar
	void setupDashboardPage(); // Initializes the dashboard page


};