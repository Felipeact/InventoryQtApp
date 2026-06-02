// DashboardPage.cpp - Implementation of the dashboard overview page
#include "DashboardPage.h"
#include "Theme.h"

#include <QHeaderView>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QStringList>
#include <algorithm>
#include <iostream>

DashboardPage::DashboardPage(
    ProductService& productService,
    ReportService& reportService,
    QWidget* parent
)
    : QWidget(parent),
    productService(productService),
    reportService(reportService)
{
    ui.setupUi(this);

    refreshDashboard();

    connect(ui.viewAllItemsButton, &QPushButton::clicked, this, [this]() {
        emit viewAllItemsRequested();
        });
}

DashboardPage::~DashboardPage()
{
}

void DashboardPage::refreshDashboard()
{
    setupItemListTable();
    setupLowStockTable();
    setupReportCards();
}

void DashboardPage::setupItemListTable()
{
    ui.itemListTable->clear();
    ui.itemListTable->setColumnCount(4);

    ui.itemListTable->setHorizontalHeaderLabels(
        QStringList() << "Name" << "Image" << "Store" << "Amount"
    );

    json products = productService.getProducts();

    int productCount = static_cast<int>(products.size());
    int rowCount = productCount < 4 ? productCount : 4;

    ui.itemListTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        auto product = products[row];

        std::string name = product.value("name", "");

        int quantity = 0;
        if (product.contains("inventory") && !product["inventory"].is_null()) {
            quantity = product["inventory"].value("quantity", 0);
        }

        ui.itemListTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::fromStdString(name))
        );

        ui.itemListTable->setItem(
            row,
            1,
            new QTableWidgetItem("Image")
        );

        ui.itemListTable->setItem(
            row,
            2,
            new QTableWidgetItem("Main Store")
        );

        ui.itemListTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::number(quantity) + " pcs")
        );
    }

    styleDashboardTable(ui.itemListTable);
}

void DashboardPage::setupLowStockTable()
{
    ui.lowStockTable->clear();
    ui.lowStockTable->setColumnCount(3);

    ui.lowStockTable->setHorizontalHeaderLabels(
        QStringList() << "Item Name" << "Store" << "Quantity"
    );

    json lowStockProducts = productService.getLowStockProducts();

    int productCount = static_cast<int>(lowStockProducts.size());
    int rowCount = productCount < 4 ? productCount : 4;

    ui.lowStockTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        auto product = lowStockProducts[row];

        std::string name = product.value("name", "");

        int quantity = 0;
        if (product.contains("inventory") && !product["inventory"].is_null()) {
            quantity = product["inventory"].value("quantity", 0);
        }

        ui.lowStockTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::fromStdString(name))
        );

        ui.lowStockTable->setItem(
            row,
            1,
            new QTableWidgetItem("Main Store")
        );

        ui.lowStockTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::number(quantity))
        );
    }

    styleDashboardTable(ui.lowStockTable);
}

void DashboardPage::styleDashboardTable(QTableWidget* table)
{
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setVisible(true);

    table->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    table->horizontalHeader()->setHighlightSections(false);
    table->horizontalHeader()->setFixedHeight(44);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setStretchLastSection(true);

    table->verticalHeader()->setDefaultSectionSize(42);

    table->setShowGrid(false);
    table->setFrameShape(QFrame::NoFrame);
    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(false);

    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void DashboardPage::setupReportCards()
{
    json inventory = reportService.getInventorySummary();
    json assets = reportService.getAssetsSummary();

    int totalProducts = inventory.value("totalProducts", 0);
    int quantityInHand = inventory.value("totalQuantity", 0);
    int lowStockCount = inventory.value("lowStockCount", 0);

    int totalAssets = assets.value("totalAssets", 0);
    int activeAssets = assets.value("activeAssets", totalAssets);
    int inactiveAssets = assets.value("inactiveAssets", 0);

    int toBeReceived = inventory.value("toBeReceived", 0);

    ui.totalItemsValue->setText(QString::number(totalProducts));
    ui.quantityInHandValue->setText(QString::number(quantityInHand));
    ui.toBeReceivedValue->setText(QString::number(toBeReceived));

    ui.itemQuantityValue->setText(QString::number(quantityInHand));
    ui.itemReceivedValue->setText(QString::number(toBeReceived));

    ui.totalAssetsValue->setText(QString::number(totalAssets));
    ui.assetActiveValue->setText(QString::number(activeAssets));
    ui.assetInactiveValue->setText(QString::number(inactiveAssets));

    // Optional: show low stock count somewhere if you add a QLabel later.
    // ui.lowStockCountValue->setText(QString::number(lowStockCount));
}

void DashboardPage::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::dashboardPageStyle(theme)
    );
}

