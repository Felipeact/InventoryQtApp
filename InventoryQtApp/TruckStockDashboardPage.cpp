#include "TruckStockDashboardPage.h"
#include "Theme.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QFrame>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>

TruckStockDashboardPage::TruckStockDashboardPage(
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);

    applyTheme(Theme::AppTheme::Dark);

    setupTables();
    setupConnections();
    loadDashboardData();
}

TruckStockDashboardPage::~TruckStockDashboardPage()
{
}

void TruckStockDashboardPage::setupConnections()
{
}

void TruckStockDashboardPage::setupTables()
{
    QList<QTableWidget*> tables = {
        ui.recentTrucksTable,
        ui.lowStockTable
    };

    for (QTableWidget* table : tables) {
        table->verticalHeader()->setVisible(false);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setShowGrid(false);
        table->setFrameShape(QFrame::NoFrame);
        table->setFocusPolicy(Qt::NoFocus);
        table->setAlternatingRowColors(false);
        table->viewport()->setAutoFillBackground(false);

        table->horizontalHeader()->setHighlightSections(false);
        table->horizontalHeader()->setDefaultAlignment(
            Qt::AlignLeft | Qt::AlignVCenter
        );

        table->horizontalHeader()->setFixedHeight(42);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        table->verticalHeader()->setDefaultSectionSize(42);

        table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}

void TruckStockDashboardPage::loadDashboardData()
{
    loadRecentTrucks();
    loadLowStockItems();
}

void TruckStockDashboardPage::refreshDashboard()
{
    loadDashboardData();
}

void TruckStockDashboardPage::loadRecentTrucks()
{
    ui.recentTrucksTable->clearContents();

    ui.recentTrucksTable->setColumnCount(5);

    ui.recentTrucksTable->setHorizontalHeaderLabels(
        QStringList()
        << "Truck"
        << "Plate"
        << "Technician"
        << "Status"
        << "Updated"
    );

    if (!truckStockService) {
        ui.recentTrucksTable->setRowCount(0);
        return;
    }

    std::vector<TruckDto> trucks =
        truckStockService->getTrucks();

    int rowCount =
        (std::min)(static_cast<int>(trucks.size()), 5);

    ui.recentTrucksTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const TruckDto& truck =
            trucks[row];

        ui.recentTrucksTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::fromStdString(truck.truckName))
        );

        ui.recentTrucksTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(truck.licensePlate))
        );

        ui.recentTrucksTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::fromStdString(truck.technicianName))
        );

        ui.recentTrucksTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::fromStdString(truck.status))
        );

        ui.recentTrucksTable->setItem(
            row,
            4,
            new QTableWidgetItem("-")
        );
    }
}

void TruckStockDashboardPage::loadLowStockItems()
{
    ui.lowStockTable->clearContents();

    ui.lowStockTable->setColumnCount(4);

    ui.lowStockTable->setHorizontalHeaderLabels(
        QStringList()
        << "Truck"
        << "Item"
        << "Current"
        << "Minimum"
    );

    if (!truckStockService) {
        ui.lowStockTable->setRowCount(0);
        return;
    }

    std::vector<LowStockItemDto> lowStockItems =
        truckStockService->getLowStockItems();

    int rowCount =
        (std::min)(static_cast<int>(lowStockItems.size()), 5);

    ui.lowStockTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const LowStockItemDto& item =
            lowStockItems[row];

        ui.lowStockTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::fromStdString(item.truckNumber))
        );

        ui.lowStockTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(item.productName))
        );

        ui.lowStockTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::number(item.currentQuantity))
        );

        ui.lowStockTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::number(item.minimumQuantity))
        );
    }
}

void TruckStockDashboardPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::truckStockDashboardPageStyle(theme)
    );
}