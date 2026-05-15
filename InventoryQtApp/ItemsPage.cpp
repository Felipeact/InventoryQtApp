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
#include <QMessageBox>
#include <QLineEdit>

// Constructor initializes the items page with table setup and data
ItemsPage::ItemsPage(ProductService& productService, QWidget* parent)
    : QWidget(parent), productService(productService)
{
    ui.setupUi(this);

    setupTable();
    loadProducts();

    connect(ui.addItemButton, &QPushButton::clicked,
        this, &ItemsPage::onAddItemClicked);

    connect(ui.itemSearchInput, &QLineEdit::textChanged,
        this, &ItemsPage::filterProducts);
}

// Destructor
ItemsPage::~ItemsPage()
{
}

void ItemsPage::refreshProducts()
{
    currentProducts = productService.getProducts();
    populateTable(currentProducts);
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
    ui.itemsTable->setAlternatingRowColors(false);
    ui.itemsTable->viewport()->setAutoFillBackground(false);

    ui.itemsTable->horizontalHeader()->setHighlightSections(false);
    ui.itemsTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui.itemsTable->horizontalHeader()->setFixedHeight(48);

    ui.itemsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.itemsTable->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Fixed);
    ui.itemsTable->setColumnWidth(8, 130);

    ui.itemsTable->verticalHeader()->setDefaultSectionSize(52);

    ui.itemsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.itemsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui.itemsTable->setStyleSheet(R"(
        QTableWidget {
            background-color: transparent;
            border: none;
            color: white;
            gridline-color: transparent;
            outline: none;
        }

        QTableWidget::item {
            background-color: transparent;
            border-bottom: 1px solid #132238;
            padding-left: 12px;
        }

        QTableWidget::item:selected {
            background-color: #132238;
        }

        QHeaderView::section {
            background-color: #0C1728;
            color: #94A3B8;
            border: none;
            border-bottom: 1px solid #1F2E45;
            padding-left: 12px;
            height: 42px;
            font-size: 12px;
            font-weight: 600;
        }
    )");

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

// Loads products from backend
void ItemsPage::loadProducts()
{
    currentProducts = productService.getProducts();
    populateTable(currentProducts);
}

// Populates the table with product data and action buttons
void ItemsPage::populateTable(const json& products)
{
    ui.itemsTable->clearContents();
    ui.itemsTable->setRowCount(static_cast<int>(products.size()));

    for (int row = 0; row < static_cast<int>(products.size()); row++) {
        auto product = products[row];

        std::string productId = product.value("id", "");
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

        QWidget* actionWidget = new QWidget();
        actionWidget->setObjectName("actionContainer");
        actionWidget->setStyleSheet("background-color: transparent; border: none;");

        QHBoxLayout* actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 0, 0);
        actionLayout->setSpacing(6);
        actionLayout->setAlignment(Qt::AlignCenter);

        QPushButton* viewButton = new QPushButton("👁");
        viewButton->setObjectName("viewButton");
        

        QPushButton* editButton = new QPushButton("✎");
        editButton->setObjectName("editButton");
        ;

        QPushButton* deleteButton = new QPushButton("🗑");
        deleteButton->setObjectName("deleteButton");
        

        connect(viewButton, &QPushButton::clicked, this, [this, product]() {
            std::string name = product.value("name", "");
            std::string barcode = product.value("barcode", "");

            int quantity = 0;
            if (product.contains("inventory") && !product["inventory"].is_null()) {
                quantity = product["inventory"].value("quantity", 0);
            }

            AddProductDialog dialog(this);
            dialog.setViewMode(
                QString::fromStdString(name),
                QString::fromStdString(barcode),
                quantity
            );

            dialog.exec();
            });

        connect(editButton, &QPushButton::clicked, this, [this, productId, product]() {
            std::string name = product.value("name", "");
            std::string barcode = product.value("barcode", "");

            int quantity = 0;

            if (product.contains("inventory") && !product["inventory"].is_null()) {
                quantity = product["inventory"].value("quantity", 0);
            }

            AddProductDialog dialog(this);
            dialog.setProductData(
                QString::fromStdString(name),
                QString::fromStdString(barcode),
                quantity
            );

            if (dialog.exec() == QDialog::Accepted) {
                bool success = productService.updateProduct(
                    productId,
                    dialog.getProductName().toStdString(),
                    dialog.getBarcode().toStdString(),
                    dialog.getQuantity()
                );

                if (success) {
                    currentProducts = productService.getProducts(true);
                    filterProducts(ui.itemSearchInput->text());
                    emit productsChanged();
                }
            }
            });

        connect(deleteButton, &QPushButton::clicked, this, [this, productId]() {
            QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                "Delete Product",
                "Are you sure you want to delete this product?",
                QMessageBox::Yes | QMessageBox::No
            );

            if (reply != QMessageBox::Yes) {
                return;
            }

            bool success = productService.deleteProduct(productId);

            if (success) {
                currentProducts = productService.getProducts(true);
                filterProducts(ui.itemSearchInput->text());
                emit productsChanged();

                QMessageBox::information(
                    this,
                    "Deleted",
                    "Product deleted successfully."
                );
            }
            else {
                QMessageBox::warning(
                    this,
                    "Error",
                    "Could not delete product."
                );
            }
            });

        actionLayout->addWidget(viewButton);
        actionLayout->addWidget(editButton);
        actionLayout->addWidget(deleteButton);

        ui.itemsTable->setCellWidget(row, 8, actionWidget);
    }
}

void ItemsPage::deleteProduct(const std::string& productId)
{
    productService.deleteProduct(productId);
}

void ItemsPage::filterProducts(const QString& searchText)
{
    currentProducts = productService.searchProducts(searchText.toStdString());
    populateTable(currentProducts);
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
            currentProducts = productService.getProducts(true);
            filterProducts(ui.itemSearchInput->text());
            emit productsChanged();
        }
    }
}