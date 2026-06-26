#pragma once

#include <QWidget>
#include <vector>
#include <string>

#include "TruckStockService.h"
#include "Theme.h"

class QLabel;
class QTableWidget;
class QFrame;

// Per-truck spending from uploaded receipts. Rejected receipts are excluded
// from the cost totals and surfaced separately. Mirrors the web/mobile
// "Truck Costs" screens. Gated behind APPROVE_RECEIPTS by the sidebar.
class TruckCostsPage : public QWidget
{
    Q_OBJECT

public:
    explicit TruckCostsPage(
        TruckStockService* truckStockService,
        const std::vector<std::string>& permissions,
        QWidget* parent = nullptr
    );

    void refreshCosts();

    void applyTheme(Theme::AppTheme theme);

private:
    TruckStockService* truckStockService = nullptr;
    std::vector<std::string> permissions;
    Theme::AppTheme currentTheme = Theme::AppTheme::Dark;

    QLabel* fleetSpendValue = nullptr;
    QLabel* receiptsCountValue = nullptr;
    QFrame* rejectedBanner = nullptr;
    QLabel* rejectedLabel = nullptr;
    QTableWidget* table = nullptr;

    void buildUi();
    void populate(const std::vector<ReceiptDto>& receipts);

    static QString money(double value);
};
