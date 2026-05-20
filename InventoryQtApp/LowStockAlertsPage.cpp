#include "LowStockAlertsPage.h"

LowStockAlertsPage::LowStockAlertsPage(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setupConnections();
    loadAlerts();
}

LowStockAlertsPage::~LowStockAlertsPage()
{
}

void LowStockAlertsPage::setupConnections()
{
    connect(ui.refreshButton, &QPushButton::clicked, this, &LowStockAlertsPage::onRefreshClicked);
}

void LowStockAlertsPage::loadAlerts()
{
    // Load low stock alerts from backend
}

void LowStockAlertsPage::refreshAlerts()
{
    loadAlerts();
}

void LowStockAlertsPage::onRefreshClicked()
{
    refreshAlerts();
}

void LowStockAlertsPage::onPageChanged(int page)
{
    // Load alerts for specified page
}
