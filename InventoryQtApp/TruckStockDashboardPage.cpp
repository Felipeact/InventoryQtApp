#include "TruckStockDashboardPage.h"
#include "Theme.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

TruckStockDashboardPage::TruckStockDashboardPage(TruckStockService* truckStockService, QWidget* parent)
    : QWidget(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);
    setupConnections();
    loadDashboardData();
}

TruckStockDashboardPage::~TruckStockDashboardPage()
{
}

void TruckStockDashboardPage::setupConnections()
{
}

void TruckStockDashboardPage::loadDashboardData()
{
    loadRecentTrucks();
    loadLowStockItems();

    ui.recentTrucksTable->horizontalHeader()->setStretchLastSection(true);
    ui.lowStockTable->horizontalHeader()->setStretchLastSection(true);

    ui.recentTrucksTable->verticalHeader()->setVisible(false);
    ui.lowStockTable->verticalHeader()->setVisible(false);
}

void TruckStockDashboardPage::refreshDashboard()
{
    loadDashboardData();
}

void TruckStockDashboardPage::loadRecentTrucks()
{
    ui.recentTrucksTable->clear();
    ui.recentTrucksTable->setColumnCount(5);

    ui.recentTrucksTable->setHorizontalHeaderLabels(
        QStringList() << "Truck" << "Plate" << "Technician" << "Status" << "Updated"
    );

    if (!truckStockService) {
        ui.recentTrucksTable->setRowCount(0);
        return;
    }

    std::vector<TruckDto> trucks = truckStockService->getTrucks();

    ui.recentTrucksTable->setRowCount(static_cast<int>(trucks.size()));

    for (int row = 0; row < static_cast<int>(trucks.size()); ++row) {
        const TruckDto& truck = trucks[row];

        ui.recentTrucksTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(truck.truckName)));
        ui.recentTrucksTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(truck.licensePlate)));
        ui.recentTrucksTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(truck.technicianName)));
        ui.recentTrucksTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(truck.status)));
        ui.recentTrucksTable->setItem(row, 4, new QTableWidgetItem("-"));
    }
}

void TruckStockDashboardPage::loadLowStockItems()
{
    ui.lowStockTable->clear();
    ui.lowStockTable->setColumnCount(4);

    ui.lowStockTable->setHorizontalHeaderLabels(
        QStringList() << "Truck" << "Item" << "Current" << "Minimum"
    );

    if (!truckStockService) {
        ui.lowStockTable->setRowCount(0);
        return;
    }

    std::vector<LowStockItemDto> lowStockItems = truckStockService->getLowStockItems();

    ui.lowStockTable->setRowCount(static_cast<int>(lowStockItems.size()));

    for (int row = 0; row < static_cast<int>(lowStockItems.size()); ++row) {
        const LowStockItemDto& item = lowStockItems[row];

        ui.lowStockTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(item.truckNumber)));
        ui.lowStockTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(item.productName)));
        ui.lowStockTable->setItem(row, 2, new QTableWidgetItem(QString::number(item.currentQuantity)));
        ui.lowStockTable->setItem(row, 3, new QTableWidgetItem(QString::number(item.minimumQuantity)));
    }
}

void TruckStockDashboardPage::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::truckPageStyle(theme)
    );
}

