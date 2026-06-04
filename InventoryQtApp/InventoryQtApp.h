// InventoryQtApp.h - Login window for the inventory management system
#pragma once
#include <DashboardWindow.h>

#include <QtWidgets/QMainWindow>
#include "ui_InventoryQtApp.h"

#include "ApiClient.h"
#include "AuthService.h"
#include "ProductService.h"
#include "AssetService.h"
#include "UserService.h"
#include "ReportService.h"
#include "TruckStockService.h"
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

private:
	Ui::InventoryQtAppClass ui;      // UI components

	ApiClient apiClient;             // API client for server communication
	AuthService authService;         // Service for authentication
	ProductService productService{ apiClient }; // Service for product operations
	AssetService assetService{ apiClient }; // Service for asset operations
	UserService userService{ apiClient }; // Service for user operations
	ReportService reportService{ apiClient }; // Service for report operations	
	TruckStockService truckStockService{ apiClient }; // Service for truck stock operation
	AutoUpdateManager* updateManager = nullptr; // Auto-update manager

	DashboardWindow* dashboardWindow = nullptr;  // Main dashboard window after login	

	// Remember Me functionality
	void saveCredentials(const QString& email, const QString& token);
	void loadSavedCredentials();
	void clearSavedCredentials();
	QString getEncryptedToken() const;
};

