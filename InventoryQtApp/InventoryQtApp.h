// InventoryQtApp.h - Login window for the inventory management system
#pragma once
#include <DashboardWindow.h>

#include <QtWidgets/QMainWindow>
#include <string>
#include <vector>
#include "ui_InventoryQtApp.h"

#include "ApiClient.h"
#include "AuthService.h"
#include "ProductService.h"
#include "AssetService.h"
#include "UserService.h"
#include "ReportService.h"
#include "TruckStockService.h"
#include "AiService.h"
#include "Theme.h"
#include "AutoUpdateManager.h"

// Main login window of the inventory management application
class InventoryQtApp : public QMainWindow
{
	Q_OBJECT

public:

	void applyTheme(Theme::AppTheme theme);
	// Constructor
	InventoryQtApp(QWidget* parent = nullptr);
	// Destructor
	~InventoryQtApp();

private slots:
	// Handles login button click and authenticates user
	void onLoginButtonClicked();
	void onForgotPasswordClicked();
	void onUpdateAvailable(const UpdateInfo& info);
	void onUpdateDownloadFinished(const QString& filePath);

private:
	Ui::InventoryQtAppClass ui;      // UI components

	ApiClient apiClient;             // API client for server communication
	AuthService authService;         // Service for authentication
	ProductService productService{ apiClient }; // Service for product operations
	AssetService assetService{ apiClient }; // Service for asset operations
	UserService userService{ apiClient }; // Service for user operations
	ReportService reportService{ apiClient }; // Service for report operations	
	TruckStockService truckStockService{ apiClient }; // Service for truck stock operation
	AiService aiService{ apiClient }; // Service for the AI assistant
	AutoUpdateManager* updateManager = nullptr; // Auto-update manager

	DashboardWindow* dashboardWindow = nullptr;  // Main dashboard window after login	

	// Completes login on the UI thread after the network call finishes on a worker.
	void onLoginFinished(
		const LoginResult& result,
		const std::string& role,
		const std::vector<std::string>& permissions,
		bool validated
	);

	// Remember Me functionality
	void saveCredentials(const QString& email, const QString& accessToken, const QString& userName);
	void openDashboard(const std::string& role, const std::vector<std::string>& permissions, const std::string& userName);
	void handleLogout();
	void loadSavedCredentials();
	void clearSavedCredentials();
};

