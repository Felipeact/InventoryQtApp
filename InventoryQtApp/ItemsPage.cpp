// ItemsPage.cpp - Implementation of the items management page
#include "ItemsPage.h"

#include <QTableWidgetItem>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QWidget>
#include <QFrame>

// Constructor initializes the items page with table setup and sample data
ItemsPage::ItemsPage(ProductService& productService, QWidget* parent)
	: QWidget(parent), productService(productService)
{
    ui.setupUi(this);

    setupTable();
	loadProducts();

    connect(ui.addItemButton, &QPushButton::clicked,
        this, &ItemsPage::onAddItemClicked);
}

// Destructor
ItemsPage::~ItemsPage()
{
}

// Configures the items table columns, headers, styling, and behavior
void ItemsPage::setupTable()
{
    ui.itemsTable->setColumnCount(9);

    ui.itemsTable->setHorizontalHeaderLabels(
        QStringList()
        << "Item Name"
        << "Image"
        << "Model"
        << "Type"
        << "Store"
        << "Amount"
        << "Project"
        << "Account"
        << "Action"
    );

    ui.itemsTable->verticalHeader()->setVisible(false);
    ui.itemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.itemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.itemsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.itemsTable->setShowGrid(false);
    ui.itemsTable->setFrameShape(QFrame::NoFrame);
    ui.itemsTable->setFocusPolicy(Qt::NoFocus);

    ui.itemsTable->horizontalHeader()->setHighlightSections(false);
    ui.itemsTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui.itemsTable->horizontalHeader()->setFixedHeight(48);

    ui.itemsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.itemsTable->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Fixed);
    ui.itemsTable->setColumnWidth(8, 130);

    ui.itemsTable->verticalHeader()->setDefaultSectionSize(52);

    ui.itemsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.itemsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    const QString premiumScrollBarStyle = R"(
    QScrollBar:vertical {
        background: transparent;
        width: 6px;
        margin: 4px 0 4px 0;
    }

    QScrollBar::handle:vertical {
        background: rgba(140, 140, 140, 0.35);
        border-radius: 3px;
        min-height: 30px;
    }

    QScrollBar::handle:vertical:hover {
        background: rgba(180, 180, 180, 0.65);
    }

    QScrollBar::handle:vertical:pressed {
        background: rgba(200, 200, 200, 0.9);
    }

    QScrollBar::add-line:vertical,
    QScrollBar::sub-line:vertical {
        height: 0;
    }

    QScrollBar::add-page:vertical,
    QScrollBar::sub-page:vertical {
        background: transparent;
    }

    QScrollBar:horizontal {
        background: transparent;
        height: 6px;
        margin: 0 4px 0 4px;
    }

    QScrollBar::handle:horizontal {
        background: rgba(140, 140, 140, 0.35);
        border-radius: 3px;
        min-width: 30px;
    }

    QScrollBar::handle:horizontal:hover {
        background: rgba(180, 180, 180, 0.65);
    }

    QScrollBar::handle:horizontal:pressed {
        background: rgba(200, 200, 200, 0.9);
    }

    QScrollBar::add-line:horizontal,
    QScrollBar::sub-line:horizontal {
        width: 0;
    }

    QScrollBar::add-page:horizontal,
    QScrollBar::sub-page:horizontal {
        background: transparent;
    }
    )";

    ui.itemsTable->verticalScrollBar()->setStyleSheet(premiumScrollBarStyle);
    ui.itemsTable->horizontalScrollBar()->setStyleSheet(premiumScrollBarStyle);

}

// Populates the table with product data and action buttons
void ItemsPage::loadProducts()
{
    json products = productService.getProducts();

    ui.itemsTable->setRowCount(static_cast<int>(products.size()));

    for (int row = 0; row < products.size(); row++) {
        auto product = products[row];

        std::string name = product.value("name", "");
        std::string barcode = product.value("barcode", "");

        int quantity = 0;

        if (product.contains("inventory") && !product["inventory"].is_null()) {
            quantity = product["inventory"].value("quantity", 0);
        }

        ui.itemsTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(name)));
        ui.itemsTable->setItem(row, 1, new QTableWidgetItem("Image"));
        ui.itemsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(barcode)));
        ui.itemsTable->setItem(row, 3, new QTableWidgetItem("Product"));
        ui.itemsTable->setItem(row, 4, new QTableWidgetItem("Main Store"));
        ui.itemsTable->setItem(row, 5, new QTableWidgetItem(QString::number(quantity) + " pcs"));
        ui.itemsTable->setItem(row, 6, new QTableWidgetItem("HQ"));
        ui.itemsTable->setItem(row, 7, new QTableWidgetItem("Activated"));
    }
}

// Opens the add product dialog and adds new items to the table
void ItemsPage::onAddItemClicked()
{
    AddProductDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        bool success = productService.createProduct(
            dialog.getProductName().toStdString(),
            dialog.getBarcode().toStdString(),
            dialog.getQuantity()
        );

        if (success) {
            productService.getProducts(true); // force refresh cache
            loadProducts();
        }
    }
}