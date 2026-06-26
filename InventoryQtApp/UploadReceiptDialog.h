#pragma once

#include <QDialog>
#include <QPoint>
#include <QString>

#include "ui_UploadReceiptDialog.h"
#include "TruckStockService.h"
#include "Theme.h"

class QMouseEvent;

class UploadReceiptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UploadReceiptDialog(
        TruckStockService* truckStockService,
        QWidget* parent = nullptr
    );

    ~UploadReceiptDialog();

    void applyTheme(Theme::AppTheme theme);

    QString getSelectedTruck() const;
    QString getSelectedTruckId() const;
    QString getTotalAmount() const;
    QString getFilePath() const;
    std::vector<ExtractedReceiptItemDto> getExtractedItems() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onBrowseClicked();
    void onUploadClicked();
    void onCancelClicked();
    void onCloseClicked();
    void onTruckSelected(int index);

private:
    Ui::UploadReceiptDialogClass ui;

    QString selectedFilePath;
    TruckStockService* truckStockService = nullptr;
    QPoint dragPosition;
    std::vector<ExtractedReceiptItemDto> extractedItems;

    void setupConnections();
    void loadTrucks();
    void updateFileDisplay(const QString& filePath);
    void loadPreview(const QString& filePath);
    void runExtraction(const QString& filePath);
};