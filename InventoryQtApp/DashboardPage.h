// DashboardPage.h - Main dashboard page showing inventory overview
#pragma once

#include <QWidget>
#include "ui_DashboardPage.h"
#include "ProductService.h"
#include "ReportService.h"

// Dashboard widget that displays a summary of inventory items
class DashboardPage : public QWidget
{
	Q_OBJECT

public:
	// Constructor
	DashboardPage(ProductService& productService, ReportService& reportService, QWidget *parent = nullptr);
	// Destructor
	~DashboardPage();

	void refreshDashboard();  // Refreshes the dashboard data and UI components

signals:
	void viewAllItemsRequested();


private:
	Ui::DashboardPageClass ui;  // UI components
	ProductService& productService;  // Reference to product service for API interactions
	ReportService& reportService;  // Reference to report service for API interactions

	// Initializes and configures the item list table
	void setupItemListTable();
	void setupReportCards();

	void setupLowStockTable();
	void styleDashboardTable(QTableWidget* table);


};

