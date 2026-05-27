#pragma once

#include <QWidget>
#include <QString>
#include <vector>

#include "ui_LowStockAlertsPage.h"
#include "TruckStockService.h"

class LowStockAlertsPage : public QWidget
{
    Q_OBJECT

public:
    explicit LowStockAlertsPage(
        TruckStockService* truckStockService,
        QWidget* parent = nullptr
    );

    ~LowStockAlertsPage();

    void refreshAlerts();

private slots:
    void onSearchChanged(const QString& text);
    void onStatusFilterChanged(int index);
    void onExportClicked();

private:
    Ui::LowStockAlertsPageClass ui;

    TruckStockService* truckStockService = nullptr;
    std::vector<LowStockItemDto> currentAlerts;

    void setupConnections();
    void loadAlerts();
    void filterAlerts();
};