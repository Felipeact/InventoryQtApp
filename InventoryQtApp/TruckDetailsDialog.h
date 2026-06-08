#pragma once

#include <QDialog>
#include <QString>
#include <QPoint>

#include "ui_TruckDetailsDialog.h"
#include "TruckStockService.h"
#include "Theme.h"

class QMouseEvent;

class TruckDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TruckDetailsDialog(
        const TruckDto& truck,
        TruckStockService* truckStockService,
        QWidget* parent = nullptr
    );

    ~TruckDetailsDialog();

    void applyTheme(
        Theme::AppTheme theme
    );

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onCloseClicked();

private:
    Ui::TruckDetailsDialogClass ui;

    TruckDto truck;
    TruckStockService* truckStockService = nullptr;
    QPoint dragPosition;

    void setupConnections();
    void setupTables();
    void loadTruckDetails();
    void loadAssignments();
    void loadLowStockItems();
};