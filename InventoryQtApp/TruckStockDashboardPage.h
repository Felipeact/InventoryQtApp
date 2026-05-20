#pragma once

#include <QWidget>
#include "ui_TruckStockDashboardPage.h"

class TruckStockDashboardPage : public QWidget
{
    Q_OBJECT

public:
    TruckStockDashboardPage(QWidget* parent = nullptr);
    ~TruckStockDashboardPage();

    void refreshDashboard();

private:
    Ui::TruckStockDashboardPageClass ui;

    void setupConnections();
    void loadDashboardData();
};
