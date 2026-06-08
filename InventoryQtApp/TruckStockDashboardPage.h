#pragma once

#include <QWidget>
#include <vector>

#include "ui_TruckStockDashboardPage.h"
#include "TruckStockService.h"
#include "Theme.h"

class TruckStockDashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit TruckStockDashboardPage(
        TruckStockService* truckStockService,
        QWidget* parent = nullptr
    );

    ~TruckStockDashboardPage();

    void refreshDashboard();

    void applyTheme(
        Theme::AppTheme theme
    );

signals:
    void viewAllTrucksRequested();
    void viewAllLowStockRequested();

private:
    struct DashboardData
    {
        std::vector<TruckDto> trucks;
        std::vector<StockTemplateDto> templates;
        std::vector<LowStockItemDto> lowStockItems;
    };

    Ui::TruckStockDashboardPageClass ui;

    TruckStockService* truckStockService = nullptr;
    bool refreshInProgress = false;
    bool pendingRefresh = false;

    DashboardData cachedData;
    bool hasCachedData = false;

    void setupConnections();
    void setupTables();

    void loadDashboardData();
    void showLoadingState();
    void applyDashboardData(const DashboardData& data);
    void populateRecentTrucks(const std::vector<TruckDto>& trucks);
    void populateLowStockItems(const std::vector<LowStockItemDto>& lowStockItems);
    void populateMetricCards(
        const std::vector<TruckDto>& trucks,
        const std::vector<StockTemplateDto>& templates,
        const std::vector<LowStockItemDto>& lowStockItems
    );
};
