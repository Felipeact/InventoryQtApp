#include "TruckStockDashboardPage.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

TruckStockDashboardPage::TruckStockDashboardPage(QWidget* parent)
    : QWidget(parent)
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
    ui.recentTrucksTable->setRowCount(4);
    ui.recentTrucksTable->setColumnCount(5);

    ui.recentTrucksTable->setHorizontalHeaderLabels(
        QStringList() << "Truck" << "Plate" << "Technician" << "Status" << "Updated"
    );

    ui.recentTrucksTable->setItem(0, 0, new QTableWidgetItem("Truck 1"));
    ui.recentTrucksTable->setItem(0, 1, new QTableWidgetItem("ABC-123"));
    ui.recentTrucksTable->setItem(0, 2, new QTableWidgetItem("John Smith"));
    ui.recentTrucksTable->setItem(0, 3, new QTableWidgetItem("Active"));
    ui.recentTrucksTable->setItem(0, 4, new QTableWidgetItem("May 22, 2024"));

    ui.recentTrucksTable->setItem(1, 0, new QTableWidgetItem("Truck 2"));
    ui.recentTrucksTable->setItem(1, 1, new QTableWidgetItem("XYZ-456"));
    ui.recentTrucksTable->setItem(1, 2, new QTableWidgetItem("Mike Johnson"));
    ui.recentTrucksTable->setItem(1, 3, new QTableWidgetItem("Active"));
    ui.recentTrucksTable->setItem(1, 4, new QTableWidgetItem("May 21, 2024"));

    ui.recentTrucksTable->setItem(2, 0, new QTableWidgetItem("Truck 3"));
    ui.recentTrucksTable->setItem(2, 1, new QTableWidgetItem("DEF-789"));
    ui.recentTrucksTable->setItem(2, 2, new QTableWidgetItem("Sarah Davis"));
    ui.recentTrucksTable->setItem(2, 3, new QTableWidgetItem("Inactive"));
    ui.recentTrucksTable->setItem(2, 4, new QTableWidgetItem("May 20, 2024"));

    ui.recentTrucksTable->setItem(3, 0, new QTableWidgetItem("Truck 4"));
    ui.recentTrucksTable->setItem(3, 1, new QTableWidgetItem("GHI-012"));
    ui.recentTrucksTable->setItem(3, 2, new QTableWidgetItem("Robert Brown"));
    ui.recentTrucksTable->setItem(3, 3, new QTableWidgetItem("Active"));
    ui.recentTrucksTable->setItem(3, 4, new QTableWidgetItem("May 18, 2024"));

    ui.lowStockTable->setRowCount(4);
    ui.lowStockTable->setColumnCount(4);

    ui.lowStockTable->setHorizontalHeaderLabels(
        QStringList() << "Truck" << "Item" << "Current" << "Minimum"
    );

    ui.lowStockTable->setItem(0, 0, new QTableWidgetItem("Truck 1"));
    ui.lowStockTable->setItem(0, 1, new QTableWidgetItem("Capacitor 45/5 μF"));
    ui.lowStockTable->setItem(0, 2, new QTableWidgetItem("1"));
    ui.lowStockTable->setItem(0, 3, new QTableWidgetItem("3"));

    ui.lowStockTable->setItem(1, 0, new QTableWidgetItem("Truck 1"));
    ui.lowStockTable->setItem(1, 1, new QTableWidgetItem("Contactor 24V"));
    ui.lowStockTable->setItem(1, 2, new QTableWidgetItem("0"));
    ui.lowStockTable->setItem(1, 3, new QTableWidgetItem("2"));

    ui.lowStockTable->setItem(2, 0, new QTableWidgetItem("Truck 2"));
    ui.lowStockTable->setItem(2, 1, new QTableWidgetItem("Fuses 30A"));
    ui.lowStockTable->setItem(2, 2, new QTableWidgetItem("2"));
    ui.lowStockTable->setItem(2, 3, new QTableWidgetItem("5"));

    ui.lowStockTable->setItem(3, 0, new QTableWidgetItem("Truck 3"));
    ui.lowStockTable->setItem(3, 1, new QTableWidgetItem("Relay 24V"));
    ui.lowStockTable->setItem(3, 2, new QTableWidgetItem("1"));
    ui.lowStockTable->setItem(3, 3, new QTableWidgetItem("2"));

    ui.recentTrucksTable->horizontalHeader()->setStretchLastSection(true);
    ui.lowStockTable->horizontalHeader()->setStretchLastSection(true);

    ui.recentTrucksTable->verticalHeader()->setVisible(false);
    ui.lowStockTable->verticalHeader()->setVisible(false);
}

void TruckStockDashboardPage::refreshDashboard()
{
    loadDashboardData();
}