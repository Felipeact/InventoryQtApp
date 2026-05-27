#pragma once

#include <QDialog>
#include "ui_UploadReceiptDialog.h"
#include "TruckStockService.h"

class UploadReceiptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UploadReceiptDialog(
        TruckStockService* truckStockService,
        QWidget* parent = nullptr
    );


    ~UploadReceiptDialog();

    QString getSelectedTruck() const;
    QString getTotalAmount() const;
    QString getFilePath() const;

    QString getSelectedTruckId() const;

private slots:
    void onBrowseClicked();
    void onUploadClicked();
    void onCancelClicked();
    void onTruckSelected(int index);

    

private:
    Ui::UploadReceiptDialogClass ui;
    QString selectedFilePath;

    TruckStockService* truckStockService = nullptr;

    void setupConnections();
    void loadTrucks();
    void updateFileDisplay(const QString& filePath);
    void loadPreview(const QString& filePath);
};
