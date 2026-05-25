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

// Main login window of the inventory management application
class InventoryQtApp : public QMainWindow
{
    Q_OBJECT

public:
	// Constructor
	InventoryQtApp(QWidget *parent = nullptr);
	// Destructor
	~InventoryQtApp();

private slots:
	// Handles login button click and authenticates user
	void onLoginButtonClicked();

private:
	Ui::InventoryQtAppClass ui;      // UI components

	ApiClient apiClient;             // API client for server communication
	AuthService authService;         // Service for authentication
	ProductService productService{ apiClient }; // Service for product operations
	AssetService assetService{ apiClient }; // Service for asset operations
	UserService userService{ apiClient }; // Service for user operations
	ReportService reportService{ apiClient }; // Service for report operations	
	TruckStockService truckStockService{ apiClient }; // Service for truck stock operation

	DashboardWindow* dashboardWindow = nullptr;  // Main dashboard window after login	

	


 
};

