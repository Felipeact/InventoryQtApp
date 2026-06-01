#pragma once

#include <QWidget>
#include <QString>
#include <vector>

#include "ui_ReceiptsPage.h"
#include "TruckStockService.h"
#include <string>
#include "Theme.h"

class ReceiptsPage : public QWidget
{
    Q_OBJECT

public:

    void applyTheme(Theme::AppTheme theme);
    explicit ReceiptsPage(
        TruckStockService* truckStockService,
        const std::vector<std::string>& permissions,
        QWidget* parent = nullptr
    );

    ~ReceiptsPage();

    void refreshReceipts();

private slots:
    void onUploadReceiptClicked();
    void onSearchChanged(const QString& text);
    void onStatusFilterChanged(int index);

private:
    Ui::ReceiptsPageClass ui;

    TruckStockService* truckStockService = nullptr;
    std::vector<ReceiptDto> currentReceipts;
    std::vector<std::string> permissions;

    void setupConnections();
    void loadReceipts();
    void filterReceipts();

    void addActionButtons(int row, const ReceiptDto& receipt);
    void onViewReceiptClicked(const ReceiptDto& receipt);
    void onApproveReceiptClicked(const ReceiptDto& receipt);

    bool hasPermission(const std::string& permission) const;
};