// DashboardPage.cpp - Implementation of the dashboard overview page
#include "DashboardPage.h"
#include <iostream>
#include <algorithm>

// Constructor initializes the dashboard page with sample inventory data
DashboardPage::DashboardPage(ProductService& productService, QWidget *parent)
	: QWidget(parent), productService(productService)   
{
	ui.setupUi(this);
    setupItemListTable();

    connect(ui.viewAllItemsButton, &QPushButton::clicked, this, [this]() {
        emit viewAllItemsRequested();
        });

}

// Destructor
DashboardPage::~DashboardPage()
{}

void DashboardPage::refreshProducts()
{
	setupItemListTable();
}

// Configures the table with columns, headers, and sample data
void DashboardPage::setupItemListTable()
{
    ui.itemListTable->setColumnCount(4); // ✅ important

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

        ui.itemListTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(name)));
        ui.itemListTable->setItem(row, 1, new QTableWidgetItem("Image"));
        ui.itemListTable->setItem(row, 2, new QTableWidgetItem("Main Store"));
        ui.itemListTable->setItem(row, 3, new QTableWidgetItem(QString::number(quantity) + " pcs"));
    }

    ui.itemListTable->verticalHeader()->setVisible(false);
    ui.itemListTable->horizontalHeader()->setVisible(true);
    ui.itemListTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui.itemListTable->horizontalHeader()->setHighlightSections(false);
    ui.itemListTable->horizontalHeader()->setFixedHeight(52);

    ui.itemListTable->setShowGrid(false);
    ui.itemListTable->setFrameShape(QFrame::NoFrame);
    ui.itemListTable->setFocusPolicy(Qt::NoFocus);
    ui.itemListTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui.itemListTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui.itemListTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui.itemListTable->setColumnWidth(0, 115);
    ui.itemListTable->setColumnWidth(1, 85);
    ui.itemListTable->setColumnWidth(2, 170);
    ui.itemListTable->setColumnWidth(3, 80);

    ui.itemListTable->verticalHeader()->setDefaultSectionSize(52);
}

