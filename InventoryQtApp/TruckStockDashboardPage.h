#pragma once

#include <QWidget>

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

private:
    Ui::TruckStockDashboardPageClass ui;

    TruckStockService* truckStockService = nullptr;

    void setupConnections();
    void setupTables();

    void loadDashboardData();
    void loadRecentTrucks();
    void loadLowStockItems();
};