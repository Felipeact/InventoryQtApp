#pragma once

#include <QDialog>
#include <QPoint>

#include "ui_ReceiptDetailsDialog.h"
#include "TruckStockService.h"
#include "Theme.h"

class QMouseEvent;

class ReceiptDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReceiptDetailsDialog(
        const ReceiptDto& receipt,
        QWidget* parent = nullptr
    );

    ~ReceiptDetailsDialog();

    void applyTheme(
        Theme::AppTheme theme
    );

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onOpenFileClicked();
    void onCloseClicked();

private:
    Ui::ReceiptDetailsDialogClass ui;

    ReceiptDto receipt;
    QPoint dragPosition;

    void setupConnections();
    void loadReceiptDetails();
    void loadPreview();

    bool isImageFile(
        const QString& filePath
    ) const;

    bool isPdfFile(
        const QString& filePath
    ) const;
};