// DashboardPage.h - Main dashboard page showing inventory overview
#pragma once

#include <QWidget>
#include "ui_DashboardPage.h"
#include "ProductService.h"

// Dashboard widget that displays a summary of inventory items
class DashboardPage : public QWidget
{
	Q_OBJECT

public:
	// Constructor
	DashboardPage(ProductService& productService, QWidget *parent = nullptr);
	// Destructor
	~DashboardPage();

	void refreshProducts();  // Refreshes the dashboard data and UI components

signals:
	void viewAllItemsRequested();


private:
	Ui::DashboardPageClass ui;  // UI components
	ProductService& productService;  // Reference to product service for API interactions

	// Initializes and configures the item list table
	void setupItemListTable();


};

