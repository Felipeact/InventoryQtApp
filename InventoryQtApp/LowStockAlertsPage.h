#pragma once

#include <QWidget>
#include <QString>
#include <vector>

#include "ui_LowStockAlertsPage.h"
#include "TruckStockService.h"
#include "Theme.h"

class LowStockAlertsPage : public QWidget
{
    Q_OBJECT

public:

    void applyTheme(Theme::AppTheme theme);
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

    void onPreviousPageClicked();
    void onNextPageClicked();
    void onPage2Clicked();

private:
    Ui::LowStockAlertsPageClass ui;

    TruckStockService* truckStockService = nullptr;

    std::vector<LowStockItemDto> currentAlerts;
    std::vector<LowStockItemDto> filteredAlerts;

    int currentPage = 1;
    int pageSize = 10;

    void setupConnections();
    void loadAlerts();
    void filterAlerts();
    void populateTable();
    void updateMetrics();
    void updatePagination();

    QString escapeCsv(const QString& value) const;
};