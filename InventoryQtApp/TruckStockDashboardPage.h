#pragma once

#include <QWidget>
#include "ui_TruckStockDashboardPage.h"
#include "TruckStockService.h"
#include "Theme.h"

class TruckStockDashboardPage : public QWidget
{
    Q_OBJECT

public:

    void applyTheme(Theme::AppTheme theme);
    explicit TruckStockDashboardPage(TruckStockService* truckStockService, QWidget* parent = nullptr);
    ~TruckStockDashboardPage();

    void refreshDashboard();

private:
    Ui::TruckStockDashboardPageClass ui;
    TruckStockService* truckStockService;

    void setupConnections();
    void loadDashboardData();
    void loadRecentTrucks();
    void loadLowStockItems();
};