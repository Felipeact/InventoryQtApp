#include "TruckStockDashboardPage.h"
#include "Theme.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QFrame>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>

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
    connect(ui.viewAllTrucksButton, &QPushButton::clicked, this, [this]() {
        emit viewAllTrucksRequested();
        });

    connect(ui.viewAllLowStockButton, &QPushButton::clicked, this, [this]() {
        emit viewAllLowStockRequested();
        });
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
    loadMetricCards();
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
        << "Stock Status"
    );

    if (!truckStockService) {
        ui.recentTrucksTable->setRowCount(0);
        return;
    }

    std::vector<TruckDto> trucks =
        truckStockService->getTrucks();

    int rowCount =
        (std::min)(static_cast<int>(trucks.size()), 5);

    if (rowCount == 0) {
        ui.recentTrucksTable->setRowCount(1);
        ui.recentTrucksTable->setItem(
            0,
            0,
            new QTableWidgetItem("No trucks found")
        );
        ui.recentTrucksTable->setSpan(0, 0, 1, 5);
        return;
    }

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

        QString stockStatus = "Normal";

        ui.recentTrucksTable->setItem(
            row,
            4,
            new QTableWidgetItem(stockStatus)
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

    if (rowCount == 0) {
        ui.lowStockTable->setRowCount(1);
        ui.lowStockTable->setItem(
            0,
            0,
            new QTableWidgetItem("No low stock truck items")
        );
        ui.lowStockTable->setSpan(0, 0, 1, 4);
        return;
    }

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

void TruckStockDashboardPage::loadMetricCards()
{
    if (!truckStockService) {
        return;
    }

    std::vector<TruckDto> trucks =
        truckStockService->getTrucks();

    std::vector<StockTemplateDto> templates =
        truckStockService->getTemplates();

    std::vector<LowStockItemDto> lowStockItems =
        truckStockService->getLowStockItems();

    int totalTrucks =
        static_cast<int>(trucks.size());

    int totalTemplates =
        static_cast<int>(templates.size());

    int lowStockCount =
        static_cast<int>(lowStockItems.size());

    int activeTrucks = 0;

    for (const TruckDto& truck : trucks) {
        QString status =
            QString::fromStdString(truck.status).toLower();

        if (status == "active") {
            activeTrucks++;
        }
    }

    ui.metricTitle1->setText("Total Trucks");
    ui.metricValue1->setText(QString::number(totalTrucks));
    ui.positiveLabel1->setText("Trucks registered");

    ui.metricTitle2->setText("Stock Templates");
    ui.metricValue2->setText(QString::number(totalTemplates));
    ui.positiveLabel2->setText("Templates available");

    ui.metricTitle3->setText("Active Trucks");
    ui.metricValue3->setText(QString::number(activeTrucks));
    ui.positiveLabel3->setText("Currently active");

    ui.metricTitle4->setText("Low Stock Items");
    ui.metricValue4->setText(QString::number(lowStockCount));
    ui.negativeLabel1->setText("Need attention");

    ui.summarySmallLabel1->setText("Total Trucks");
    ui.summaryValue1->setText(QString::number(totalTrucks));
    ui.summaryPositive1->setText("Registered");

    ui.summarySmallLabel2->setText("Templates");
    ui.summaryValue2->setText(QString::number(totalTemplates));
    ui.summaryNegative1->setText("Available");

    ui.summarySmallLabel3->setText("Active Trucks");
    ui.summaryValue3->setText(QString::number(activeTrucks));
    ui.summaryPositive2->setText("Active");

    ui.summarySmallLabel4->setText("Low Stock");
    ui.summaryValue4->setText(QString::number(lowStockCount));
    ui.summaryNegative2->setText("Alerts");
}

void TruckStockDashboardPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::truckStockDashboardPageStyle(theme)
    );
}