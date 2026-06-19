#pragma once

#include <QWidget>
#include <QString>
#include <vector>
#include <string>

#include "ui_LowStockAlertsPage.h"
#include "TruckStockService.h"
#include "Theme.h"

class LowStockAlertsPage : public QWidget
{
    Q_OBJECT

public:
    explicit LowStockAlertsPage(
        TruckStockService* truckStockService,
        const std::vector<std::string>& permissions,
        QWidget* parent = nullptr
    );

    ~LowStockAlertsPage();

    void refreshAlerts();

    void applyTheme(
        Theme::AppTheme theme
    );

    void setSearchText(const QString& text);

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

    std::vector<std::string> permissions;
    bool hasPermission(const std::string& permission) const;

    std::vector<LowStockItemDto> currentAlerts;
    std::vector<LowStockItemDto> filteredAlerts;

    int currentPage = 1;
    int pageSize = 10;

    void setupConnections();
    void setupTable();

    void loadAlerts();
    void filterAlerts();
    void populateTable();
    void updateMetrics();
    void updatePagination();

    void addActionButtons(
        int row,
        const LowStockItemDto& alert
    );

    void onRestockClicked(
        const LowStockItemDto& alert
    );

    void onCreateReceiptClicked(
        const LowStockItemDto& alert
    );

    void onCreateAssignmentClicked(
        const LowStockItemDto& alert
    );

    QString escapeCsv(
        const QString& value
    ) const;
};