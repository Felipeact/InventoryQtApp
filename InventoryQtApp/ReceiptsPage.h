#pragma once

#include <QWidget>
#include <QString>
#include <vector>

#include "ui_ReceiptsPage.h"
#include "TruckStockService.h"

class ReceiptsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiptsPage(
        TruckStockService* truckStockService,
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

    void setupConnections();
    void loadReceipts();
    void filterReceipts();
};