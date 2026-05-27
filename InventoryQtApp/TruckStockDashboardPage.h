#pragma once

#include <QWidget>
#include "ui_TruckStockDashboardPage.h"
#include "TruckStockService.h"

class TruckStockDashboardPage : public QWidget
{
    Q_OBJECT

public:
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