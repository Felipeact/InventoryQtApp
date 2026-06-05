#pragma once

#include <QDialog>

#include "ui_NotificationDialog.h"
#include "TruckStockService.h"
#include "Theme.h"

class NotificationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NotificationDialog(
        TruckStockService* truckStockService,
        QWidget* parent = nullptr
    );

    ~NotificationDialog();

    void applyTheme(Theme::AppTheme theme);

signals:
    void openLowStockRequested();
    void openReceiptsRequested();

private slots:
    void onOpenLowStockClicked();
    void onOpenReceiptsClicked();
    void onCloseClicked();

private:
    Ui::NotificationDialogClass ui;

    TruckStockService* truckStockService = nullptr;

    void setupConnections();
    void loadNotifications();
};