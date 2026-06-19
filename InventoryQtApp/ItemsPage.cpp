#include "ItemsPage.h"
#include "IconUtil.h"
#include "Theme.h"
#include "ItemDetailsDialog.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QComboBox>
#include <QFileInfo>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMetaObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPointer>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QThread>
#include <QUrl>
#include <QWidget>

ItemsPage::ItemsPage(
    ProductService& productService,
    const std::vector<std::string>& permissions,
    QWidget* parent
)
    : QWidget(parent),
    productService(productService),
    permissions(permissions)
{
    ui.setupUi(this);

    applyTheme(Theme::AppTheme::Dark);

    ui.pageSizeCombo->clear();
    ui.pageSizeCombo->addItem("5 / page");
    ui.pageSizeCombo->addItem("10 / page");
    ui.pageSizeCombo->addItem("20 / page");
    ui.pageSizeCombo->setCurrentText("10 / page");

    setupTable();
    setupConnections();

    // Creating, editing and deleting products all require ADD_PRODUCT on the
    // backend; viewing only needs VIEW_STOCK. Hide write actions for users
    // (e.g. WAREHOUSE) who can view stock but not manage it.
    ui.addItemButton->setVisible(hasPermission("ADD_PRODUCT"));

    loadProducts();
}

bool ItemsPage::hasPermission(
    const std::string& permission
) const
{
    return std::find(
        permissions.begin(),
        permissions.end(),
        permission
    ) != permissions.end();
}

ItemsPage::~ItemsPage()
{
}

void ItemsPage::setupConnections()
{
    connect(
        ui.addItemButton,
        &QPushButton::clicked,
        this,
        &ItemsPage::onAddItemClicked
    );

    connect(
        ui.itemSearchInput,
        &QLineEdit::textChanged,
        this,
        &ItemsPage::filterProducts
    );

    connect(
        ui.filterItemButton,
        &QPushButton::clicked,
        this,
        &ItemsPage::onFilterButtonClicked
    );

    connect(
        ui.nextPageButton,
        &QPushButton::clicked,
        this,
        &ItemsPage::onNextPageClicked
    );

    connect(
        ui.prevPageButton,
        &QPushButton::clicked,
        this,
        &ItemsPage::onPrevPageClicked
    );

    connect(
        ui.page2Button,
        &QPushButton::clicked,
        this,
        &ItemsPage::onPage2Clicked
    );

    connect(
        ui.pageSizeCombo,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &ItemsPage::onPageSizeChanged
    );
}

void ItemsPage::refreshProducts()
{
    QPointer<ItemsPage> self(this);

    QThread* worker =
        QThread::create([self]() {
        if (!self) {
            return;
        }

        json products =
            self->productService.getProducts(true);

        QMetaObject::invokeMethod(
            self,
            [self, products]() {
                if (!self) {
                    return;
                }

                self->currentProducts = products;
                self->applyFilters();
            },
            Qt::QueuedConnection
        );
            });

    connect(
        worker,
        &QThread::finished,
        worker,
        &QObject::deleteLater
    );

    worker->start();
}

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
    ui.itemsTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    ui.itemsTable->horizontalHeader()->setFixedHeight(48);
    ui.itemsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.itemsTable->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Fixed);

    ui.itemsTable->setColumnWidth(8, 130);
    ui.itemsTable->verticalHeader()->setDefaultSectionSize(58);

    ui.itemsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.itemsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void ItemsPage::loadProducts()
{
    QPointer<ItemsPage> self(this);

    QThread* worker =
        QThread::create([self]() {
        if (!self) {
            return;
        }

        json products =
            self->productService.getProducts();

        QMetaObject::invokeMethod(
            self,
            [self, products]() {
                if (!self) {
                    return;
                }

                self->currentProducts = products;
                self->applyFilters();
            },
            Qt::QueuedConnection
        );
            });

    connect(
        worker,
        &QThread::finished,
        worker,
        &QObject::deleteLater
    );

    worker->start();
}

void ItemsPage::populateTable(
    const json& products
)
{
    ui.itemsTable->setUpdatesEnabled(false);
    ui.itemsTable->clearContents();
    ui.itemsTable->clearSpans();

    int rowCount =
        products.is_array()
        ? static_cast<int>(products.size())
        : 0;

    if (rowCount == 0) {
        ui.itemsTable->setRowCount(1);

        ui.itemsTable->setItem(
            0,
            0,
            new QTableWidgetItem("No items found")
        );

        ui.itemsTable->setSpan(0, 0, 1, 9);
        ui.itemsTable->setUpdatesEnabled(true);
        return;
    }

    ui.itemsTable->setRowCount(rowCount);

    const bool canManageProducts = hasPermission("ADD_PRODUCT");

    for (int row = 0; row < rowCount; row++) {
        json product =
            products[row];

        std::string productId =
            product.value("id", "");

        QString name =
            getProductString(product, "name");

        QString barcode =
            getProductString(product, "barcode");

        QString model =
            getProductString(product, "model", barcode);

        int quantity =
            getProductQuantity(product);

        QString type =
            getProductString(product, "type", "Product");

        QString store =
            getProductString(product, "location", "Main Store");

        QString project =
            getProductString(product, "project", "");

        QString account =
            getProductString(product, "account", getProductString(product, "status", "Active"));

        ui.itemsTable->setItem(
            row,
            0,
            new QTableWidgetItem(name)
        );

        setImageCell(
            row,
            1,
            getProductImageValue(product)
        );

        ui.itemsTable->setItem(
            row,
            2,
            new QTableWidgetItem(model)
        );

        ui.itemsTable->setItem(
            row,
            3,
            new QTableWidgetItem(type)
        );

        ui.itemsTable->setItem(
            row,
            4,
            new QTableWidgetItem(store)
        );

        ui.itemsTable->setItem(
            row,
            5,
            new QTableWidgetItem(
                QString::number(quantity) + " pcs"
            )
        );

        ui.itemsTable->setItem(
            row,
            6,
            new QTableWidgetItem(project)
        );

        ui.itemsTable->setItem(
            row,
            7,
            new QTableWidgetItem(account)
        );

        QWidget* actionWidget =
            new QWidget(this);

        actionWidget->setObjectName("actionContainer");

        QHBoxLayout* actionLayout =
            new QHBoxLayout(actionWidget);

        actionLayout->setContentsMargins(0, 0, 0, 0);
        actionLayout->setSpacing(6);
        actionLayout->setAlignment(Qt::AlignCenter);

        QPushButton* viewButton =
            new QPushButton("👁", actionWidget);

        viewButton->setObjectName("viewButton");
        IconUtil::iconifyActionButton(viewButton);

        QPushButton* editButton =
            new QPushButton("✎", actionWidget);

        editButton->setObjectName("editButton");
        IconUtil::iconifyActionButton(editButton);

        QPushButton* deleteButton =
            new QPushButton("🗑", actionWidget);

        deleteButton->setObjectName("deleteButton");
        IconUtil::iconifyActionButton(deleteButton);

        connect(
            viewButton,
            &QPushButton::clicked,
            this,
            [this, product]() {
                ItemDetailsDialog dialog(
                    product,
                    this
                );

                dialog.exec();
            }
        );

        connect(
            editButton,
            &QPushButton::clicked,
            this,
            [this, productId, product]() {
                QString name =
                    getProductString(product, "name");

                QString barcode =
                    getProductString(product, "barcode");

                int quantity =
                    getProductQuantity(product);

                QString store =
                    getProductString(product, "location", "Main Store");

                QString description =
                    getProductString(product, "description", "");

                AddProductDialog dialog(this);

                dialog.setProductData(
                    name,
                    barcode,
                    quantity,
                    store,
                    description
                );

                if (dialog.exec() == QDialog::Accepted) {
                    bool success =
                        productService.updateProduct(
                            productId,
                            dialog.getProductName().toStdString(),
                            dialog.getBarcode().toStdString(),
                            dialog.getQuantity(),
                            dialog.getStore().toStdString(),
                            dialog.getDescription().toStdString()
                        );

                    if (success) {
                        currentProducts =
                            productService.getProducts(true);

                        applyFilters();

                        emit productsChanged();
                    }
                }
            }
        );

        connect(
            deleteButton,
            &QPushButton::clicked,
            this,
            [this, productId]() {
                QMessageBox::StandardButton reply =
                    QMessageBox::question(
                        this,
                        "Delete Product",
                        "Are you sure you want to delete this product?",
                        QMessageBox::Yes | QMessageBox::No
                    );

                if (reply != QMessageBox::Yes) {
                    return;
                }

                bool success =
                    productService.deleteProduct(productId);

                if (success) {
                    currentProducts =
                        productService.getProducts(true);

                    applyFilters();

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
            }
        );

        if (!canManageProducts) {
            editButton->hide();
            deleteButton->hide();
        }

        actionLayout->addWidget(viewButton);
        actionLayout->addWidget(editButton);
        actionLayout->addWidget(deleteButton);

        ui.itemsTable->setCellWidget(
            row,
            8,
            actionWidget
        );
    }

    ui.itemsTable->setUpdatesEnabled(true);
}

void ItemsPage::deleteProduct(
    const std::string& productId
)
{
    productService.deleteProduct(productId);
}

void ItemsPage::filterProducts(
    const QString& searchText
)
{
    Q_UNUSED(searchText);

    applyFilters();
}

void ItemsPage::applyFilters()
{
    filteredProducts =
        json::array();

    QString searchText =
        ui.itemSearchInput->text().trimmed();

    for (const auto& product : currentProducts) {
        if (
            productMatchesSearch(product, searchText) &&
            productMatchesAdvancedFilters(product)
            ) {
            filteredProducts.push_back(product);
        }
    }

    currentPage = 1;

    populateTable(
        getCurrentPageProducts()
    );

    updatePagination();
}

bool ItemsPage::productMatchesSearch(
    const json& product,
    const QString& searchText
) const
{
    if (searchText.isEmpty()) {
        return true;
    }

    QString name =
        getProductString(product, "name");

    QString barcode =
        getProductString(product, "barcode");

    QString type =
        getProductString(product, "type", "Product");

    QString store =
        getProductString(product, "location", "Main Store");

    return name.contains(searchText, Qt::CaseInsensitive) ||
        barcode.contains(searchText, Qt::CaseInsensitive) ||
        type.contains(searchText, Qt::CaseInsensitive) ||
        store.contains(searchText, Qt::CaseInsensitive);
}

bool ItemsPage::productMatchesAdvancedFilters(
    const json& product
) const
{
    if (!advancedFilters.enabled) {
        return true;
    }

    int quantity =
        getProductQuantity(product);

    if (
        advancedFilters.useMinQuantity &&
        quantity < advancedFilters.minQuantity
        ) {
        return false;
    }

    if (
        advancedFilters.useMaxQuantity &&
        quantity > advancedFilters.maxQuantity
        ) {
        return false;
    }

    if (!advancedFilters.storeText.isEmpty()) {
        QString store =
            getProductString(product, "location", "Main Store");

        if (!store.contains(advancedFilters.storeText, Qt::CaseInsensitive)) {
            return false;
        }
    }

    if (!advancedFilters.typeText.isEmpty()) {
        QString type =
            getProductString(product, "type", "Product");

        if (!type.contains(advancedFilters.typeText, Qt::CaseInsensitive)) {
            return false;
        }
    }

    if (advancedFilters.stockStatus != "All") {
        int minimumQuantity =
            getProductMinimumQuantity(product);

        bool outOfStock =
            quantity <= 0;

        bool lowStock =
            quantity > 0 &&
            minimumQuantity > 0 &&
            quantity <= minimumQuantity;

        bool inStock =
            quantity > 0 &&
            !lowStock;

        if (
            advancedFilters.stockStatus == "Out of Stock" &&
            !outOfStock
            ) {
            return false;
        }

        if (
            advancedFilters.stockStatus == "Low Stock" &&
            !lowStock
            ) {
            return false;
        }

        if (
            advancedFilters.stockStatus == "In Stock" &&
            !inStock
            ) {
            return false;
        }
    }

    return true;
}

int ItemsPage::getProductQuantity(
    const json& product
) const
{
    if (
        product.contains("inventory") &&
        !product["inventory"].is_null()
        ) {
        return product["inventory"].value("quantity", 0);
    }

    return product.value("quantity", 0);
}

int ItemsPage::getProductMinimumQuantity(
    const json& product
) const
{
    if (
        product.contains("inventory") &&
        !product["inventory"].is_null()
        ) {
        if (product["inventory"].contains("minimumQuantity")) {
            return product["inventory"].value("minimumQuantity", 0);
        }

        if (product["inventory"].contains("minimumStock")) {
            return product["inventory"].value("minimumStock", 0);
        }

        if (product["inventory"].contains("reorderLevel")) {
            return product["inventory"].value("reorderLevel", 0);
        }
    }

    if (product.contains("minimumQuantity")) {
        return product.value("minimumQuantity", 0);
    }

    if (product.contains("minimumStock")) {
        return product.value("minimumStock", 0);
    }

    if (product.contains("reorderLevel")) {
        return product.value("reorderLevel", 0);
    }

    return 0;
}

QString ItemsPage::getProductString(
    const json& product,
    const std::string& key,
    const QString& fallback
) const
{
    if (
        product.contains(key) &&
        product[key].is_string()
        ) {
        std::string value =
            product.value(key, "");

        if (!value.empty()) {
            return QString::fromStdString(value);
        }
    }

    return fallback;
}

QString ItemsPage::getProductImageValue(
    const json& product
) const
{
    if (product.contains("imageUrl")) {
        return QString::fromStdString(
            product.value("imageUrl", "")
        );
    }

    if (product.contains("image")) {
        return QString::fromStdString(
            product.value("image", "")
        );
    }

    if (product.contains("photoUrl")) {
        return QString::fromStdString(
            product.value("photoUrl", "")
        );
    }

    if (product.contains("thumbnailUrl")) {
        return QString::fromStdString(
            product.value("thumbnailUrl", "")
        );
    }

    return "";
}

void ItemsPage::setImageCell(
    int row,
    int column,
    const QString& imagePathOrUrl
)
{
    QLabel* imageLabel =
        new QLabel(this);

    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setFixedSize(52, 40);
    imageLabel->setText("Image");

    ui.itemsTable->setCellWidget(
        row,
        column,
        imageLabel
    );

    QString imageValue =
        imagePathOrUrl.trimmed();

    if (imageValue.isEmpty()) {
        return;
    }

    if (
        imageValue.startsWith("http://", Qt::CaseInsensitive) ||
        imageValue.startsWith("https://", Qt::CaseInsensitive)
        ) {
        QNetworkAccessManager* manager =
            new QNetworkAccessManager(imageLabel);

        QNetworkReply* reply =
            manager->get(
                QNetworkRequest(
                    QUrl(imageValue)
                )
            );

        connect(
            reply,
            &QNetworkReply::finished,
            imageLabel,
            [reply, imageLabel]() {
                QByteArray imageData =
                    reply->readAll();

                QPixmap pixmap;

                if (pixmap.loadFromData(imageData)) {
                    imageLabel->setText("");

                    imageLabel->setPixmap(
                        pixmap.scaled(
                            imageLabel->size(),
                            Qt::KeepAspectRatio,
                            Qt::SmoothTransformation
                        )
                    );
                }

                reply->deleteLater();
            }
        );

        return;
    }

    QFileInfo fileInfo(imageValue);

    if (!fileInfo.exists()) {
        return;
    }

    QPixmap pixmap(imageValue);

    if (pixmap.isNull()) {
        return;
    }

    imageLabel->setText("");

    imageLabel->setPixmap(
        pixmap.scaled(
            imageLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        )
    );
}

void ItemsPage::onAddItemClicked()
{
    AddProductDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        bool success =
            productService.createProduct(
                dialog.getProductName().toStdString(),
                dialog.getBarcode().toStdString(),
                dialog.getQuantity(),
                dialog.getStore().toStdString(),
                dialog.getDescription().toStdString()
            );

        if (success) {
            currentProducts =
                productService.getProducts(true);

            applyFilters();

            emit productsChanged();
        }
    }
}

json ItemsPage::getCurrentPageProducts() const
{
    json pageProducts =
        json::array();

    int totalItems =
        static_cast<int>(filteredProducts.size());

    int startIndex =
        (currentPage - 1) * pageSize;

    int endIndex =
        (std::min)(startIndex + pageSize, totalItems);

    for (int i = startIndex; i < endIndex; i++) {
        pageProducts.push_back(
            filteredProducts[i]
        );
    }

    return pageProducts;
}

void ItemsPage::updatePagination()
{
    int totalItems =
        static_cast<int>(filteredProducts.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage > totalPages) {
        currentPage = totalPages;
    }

    int startItem =
        totalItems == 0
        ? 0
        : ((currentPage - 1) * pageSize) + 1;

    int endItem =
        (std::min)(currentPage * pageSize, totalItems);

    ui.paginationLabel->setText(
        QString("Showing %1 to %2 of %3 items")
        .arg(startItem)
        .arg(endItem)
        .arg(totalItems)
    );

    ui.activePageButton->setText(
        QString::number(currentPage)
    );

    ui.page2Button->setText(
        QString::number(currentPage + 1)
    );

    ui.prevPageButton->setEnabled(
        currentPage > 1
    );

    ui.nextPageButton->setEnabled(
        currentPage < totalPages
    );

    ui.page2Button->setVisible(
        currentPage < totalPages
    );
}

void ItemsPage::onFilterButtonClicked()
{
    ItemAdvancedFilterDialog dialog(
        advancedFilters,
        this
    );

    if (dialog.exec() == QDialog::Accepted) {
        advancedFilters =
            dialog.getFilters();

        applyFilters();
    }
}

void ItemsPage::onNextPageClicked()
{
    int totalItems =
        static_cast<int>(filteredProducts.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage < totalPages) {
        currentPage++;

        populateTable(
            getCurrentPageProducts()
        );

        updatePagination();
    }
}

void ItemsPage::onPrevPageClicked()
{
    if (currentPage > 1) {
        currentPage--;

        populateTable(
            getCurrentPageProducts()
        );

        updatePagination();
    }
}

void ItemsPage::onPage2Clicked()
{
    int totalItems =
        static_cast<int>(filteredProducts.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage + 1 <= totalPages) {
        currentPage++;

        populateTable(
            getCurrentPageProducts()
        );

        updatePagination();
    }
}

void ItemsPage::onPageSizeChanged(
    int index
)
{
    QString text =
        ui.pageSizeCombo->itemText(index);

    if (text.startsWith("5")) {
        pageSize = 5;
    }
    else if (text.startsWith("20")) {
        pageSize = 20;
    }
    else {
        pageSize = 10;
    }

    currentPage = 1;

    populateTable(
        getCurrentPageProducts()
    );

    updatePagination();
}

void ItemsPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::itemsPageStyle(theme)
    );
}

void ItemsPage::setSearchText(
    const QString& text
)
{
    ui.itemSearchInput->setText(text);
}