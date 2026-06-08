#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLabel>

#include "ui_DashboardPage.h"
#include "ProductService.h"
#include "ReportService.h"
#include "Theme.h"

class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    DashboardPage(
        ProductService& productService,
        ReportService& reportService,
        QWidget* parent = nullptr
    );

    ~DashboardPage();

    void refreshDashboard();

    void applyTheme(
        Theme::AppTheme theme
    );

signals:
    void viewAllItemsRequested();
    void viewAllLowStockRequested();

private:
    Ui::DashboardPageClass ui;

    ProductService& productService;
    ReportService& reportService;

    void setupConnections();

    void setupItemListTable(
        const json& products
    );

    void setupLowStockTable(
        const json& lowStockProducts
    );

    void setupReportCards(
        const json& inventory,
        const json& assets,
        const json& lowStockProducts
    );

    void styleDashboardTable(
        QTableWidget* table
    );

    void setProductImageCell(
        int row,
        int column,
        const QString& imagePathOrUrl
    );

    QString getProductImageUrl(
        const json& product
    ) const;
};