#pragma once

#include <QWidget>
#include <QString>
#include <vector>
#include <string>

#include "ui_ReceiptsPage.h"
#include "TruckStockService.h"
#include "Theme.h"

class ReceiptsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiptsPage(
        TruckStockService* truckStockService,
        const std::vector<std::string>& permissions,
        QWidget* parent = nullptr
    );

    ~ReceiptsPage();

    void refreshReceipts();

    void applyTheme(
        Theme::AppTheme theme
    );

private slots:
    void onUploadReceiptClicked();
    void onSearchChanged(const QString& text);
    void onStatusFilterChanged(int index);

signals:
    void receiptsChanged();


private:
    Ui::ReceiptsPageClass ui;

    TruckStockService* truckStockService = nullptr;

    std::vector<ReceiptDto> currentReceipts;
    std::vector<ReceiptDto> filteredReceipts;
    std::vector<std::string> permissions;

    void setupConnections();
    void setupTable();

    void loadReceipts();
    void filterReceipts();
    void populateTable();

    void addActionButtons(
        int row,
        const ReceiptDto& receipt
    );

    void onViewReceiptClicked(
        const ReceiptDto& receipt
    );

    void onApproveReceiptClicked(
        const ReceiptDto& receipt
    );

    bool hasPermission(
        const std::string& permission
    ) const;
};