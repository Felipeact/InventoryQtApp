#pragma once

#include <QWidget>
#include "ui_LowStockAlertsPage.h"

class LowStockAlertsPage : public QWidget
{
    Q_OBJECT

public:
    LowStockAlertsPage(QWidget* parent = nullptr);
    ~LowStockAlertsPage();

    void refreshAlerts();

private slots:
    void onRefreshClicked();
    void onPageChanged(int page);

private:
    Ui::LowStockAlertsPageClass ui;

    void setupConnections();
    void loadAlerts();
};
