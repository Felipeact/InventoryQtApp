#include "DashboardPage.h"
#include "Theme.h"

#include <algorithm>
#include <iostream>

#include <QAbstractItemView>
#include <QFileInfo>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QLocale>
#include <QMetaObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPixmap>
#include <QPushButton>
#include <QStringList>
#include <QTableWidgetItem>
#include <QThread>
#include <QPointer>
#include <QUrl>

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

    setupConnections();
    refreshDashboard();
}

DashboardPage::~DashboardPage()
{
}

void DashboardPage::setupConnections()
{
    connect(
        ui.viewAllItemsButton,
        &QPushButton::clicked,
        this,
        [this]() {
            emit viewAllItemsRequested();
        }
    );

    connect(
        ui.viewAllLowStockButton,
        &QPushButton::clicked,
        this,
        [this]() {
            emit viewAllLowStockRequested();
        }
    );
}

void DashboardPage::refreshDashboard()
{
    QPointer<DashboardPage> self(this);

    ProductService* products =
        &productService;

    ReportService* reports =
        &reportService;

    QThread* worker =
        QThread::create([self, products, reports]() {
        json allProducts =
            products->getProducts();

        json lowStockProducts =
            products->getLowStockProducts();

        json inventory =
            reports->getInventorySummary();

        json assets =
            reports->getAssetsSummary();

        if (!self) {
            return;
        }

        QMetaObject::invokeMethod(
            self,
            [self, allProducts, lowStockProducts, inventory, assets]() {
                if (!self) {
                    return;
                }

                self->setupItemListTable(
                    allProducts
                );

                self->setupLowStockTable(
                    lowStockProducts
                );

                self->setupReportCards(
                    inventory,
                    assets,
                    lowStockProducts
                );
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

void DashboardPage::setupItemListTable(
    const json& products
)
{
    ui.itemListTable->clearContents();
    ui.itemListTable->clearSpans();

    ui.itemListTable->setColumnCount(4);

    ui.itemListTable->setHorizontalHeaderLabels(
        QStringList()
        << "Name"
        << "Image"
        << "Store"
        << "Amount"
    );

    int productCount =
        products.is_array()
        ? static_cast<int>(products.size())
        : 0;

    int rowCount =
        (std::min)(productCount, 4);

    if (rowCount == 0) {
        ui.itemListTable->setRowCount(1);

        ui.itemListTable->setItem(
            0,
            0,
            new QTableWidgetItem("No recent items found")
        );

        ui.itemListTable->setSpan(
            0,
            0,
            1,
            4
        );

        styleDashboardTable(
            ui.itemListTable
        );

        return;
    }

    ui.itemListTable->setRowCount(
        rowCount
    );

    for (int row = 0; row < rowCount; row++) {
        const json& product =
            products[row];

        QString name =
            QString::fromStdString(
                product.value("name", "")
            );

        int quantity = 0;

        if (
            product.contains("inventory") &&
            !product["inventory"].is_null()
            ) {
            quantity =
                product["inventory"].value("quantity", 0);
        }

        QString imageUrl =
            getProductImageUrl(product);

        QString location =
            QString::fromStdString(
                product.value("location", "Unknown Store")
            );

        ui.itemListTable->setItem(
            row,
            0,
            new QTableWidgetItem(name)
        );

        setProductImageCell(
            row,
            1,
            imageUrl
        );

        ui.itemListTable->setItem(
            row,
            2,
            new QTableWidgetItem(location)
        );

        ui.itemListTable->setItem(
            row,
            3,
            new QTableWidgetItem(
                QString::number(quantity) + " pcs"
            )
        );
    }

    styleDashboardTable(
        ui.itemListTable
    );
}

void DashboardPage::setupLowStockTable(
    const json& lowStockProducts
)
{
    ui.lowStockTable->clearContents();
    ui.lowStockTable->clearSpans();

    ui.lowStockTable->setColumnCount(3);

    ui.lowStockTable->setHorizontalHeaderLabels(
        QStringList()
        << "Item Name"
        << "Store"
        << "Quantity"
    );

    int productCount =
        lowStockProducts.is_array()
        ? static_cast<int>(lowStockProducts.size())
        : 0;

    ui.lowStockCountValue->setText(
        QString::number(productCount)
    );

    int rowCount =
        (std::min)(productCount, 4);

    if (rowCount == 0) {
        ui.lowStockTable->setRowCount(1);

        ui.lowStockTable->setItem(
            0,
            0,
            new QTableWidgetItem("No low stock items")
        );

        ui.lowStockTable->setSpan(
            0,
            0,
            1,
            3
        );

        styleDashboardTable(
            ui.lowStockTable
        );

        return;
    }

    ui.lowStockTable->setRowCount(
        rowCount
    );

    for (int row = 0; row < rowCount; row++) {
        const json& product =
            lowStockProducts[row];

        QString name =
            QString::fromStdString(
                product.value("name", "")
            );

        int quantity = 0;

        if (
            product.contains("inventory") &&
            !product["inventory"].is_null()
            ) {
            quantity =
                product["inventory"].value("quantity", 0);
        }

        QString location =
            QString::fromStdString(
                product.value("location", "Unknown Store")
            );

        ui.lowStockTable->setItem(
            row,
            0,
            new QTableWidgetItem(name)
        );

        ui.lowStockTable->setItem(
            row,
            1,
            new QTableWidgetItem(location)
        );

        ui.lowStockTable->setItem(
            row,
            2,
            new QTableWidgetItem(
                QString::number(quantity)
            )
        );
    }

    styleDashboardTable(
        ui.lowStockTable
    );
}

void DashboardPage::setupReportCards(
    const json& inventory,
    const json& assets,
    const json& lowStockProducts
)
{
    int totalProducts =
        inventory.value("totalProducts", 0);

    int quantityInHand =
        inventory.value("totalQuantity", 0);

    int toBeReceived =
        inventory.value("toBeReceived", 0);

    int lowStockCount =
        inventory.value("lowStockCount", 0);

    if (
        lowStockCount == 0 &&
        lowStockProducts.is_array()
        ) {
        lowStockCount =
            static_cast<int>(lowStockProducts.size());
    }

    int totalAssets =
        assets.value("totalAssets", 0);

    int activeAssets =
        assets.value("activeAssets", totalAssets);

    int inactiveAssets =
        assets.value("inactiveAssets", 0);

    QLocale locale;

    ui.totalItemsValue->setText(
        locale.toString(totalProducts)
    );

    ui.quantityInHandValue->setText(
        locale.toString(quantityInHand)
    );

    ui.toBeReceivedValue->setText(
        locale.toString(toBeReceived)
    );

    ui.lowStockCountValue->setText(
        locale.toString(lowStockCount)
    );

    ui.itemQuantityValue->setText(
        locale.toString(quantityInHand)
    );

    ui.itemReceivedValue->setText(
        locale.toString(toBeReceived)
    );

    ui.totalAssetsValue->setText(
        locale.toString(totalAssets)
    );

    ui.assetActiveValue->setText(
        locale.toString(activeAssets)
    );

    ui.assetInactiveValue->setText(
        locale.toString(inactiveAssets)
    );

    ui.totalItemsChange->setText(
        "Products tracked"
    );

    ui.totalAssetsChange->setText(
        "Assets registered"
    );

    ui.quantityChange->setText(
        "Units available"
    );

    ui.receivedChange->setText(
        "Pending receipts"
    );
}

void DashboardPage::styleDashboardTable(
    QTableWidget* table
)
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

    table->verticalHeader()->setDefaultSectionSize(56);

    table->setShowGrid(false);
    table->setFrameShape(QFrame::NoFrame);
    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(false);

    table->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    table->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
    );
}

void DashboardPage::setProductImageCell(
    int row,
    int column,
    const QString& imagePathOrUrl
)
{
    QLabel* imageLabel =
        new QLabel();

    imageLabel->setAlignment(
        Qt::AlignCenter
    );

    imageLabel->setFixedSize(
        58,
        42
    );

    imageLabel->setText(
        "No image"
    );

    imageLabel->setScaledContents(false);

    ui.itemListTable->setCellWidget(
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

        QNetworkRequest request{ QUrl(imageValue) };

        QNetworkReply* reply =
            manager->get(request);

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
                else {
                    imageLabel->setText("No image");
                }

                reply->deleteLater();
            }
        );

        return;
    }

    QFileInfo fileInfo(imageValue);

    if (!fileInfo.exists()) {
        imageLabel->setText("No image");
        return;
    }

    QPixmap pixmap(imageValue);

    if (pixmap.isNull()) {
        imageLabel->setText("No image");
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

QString DashboardPage::getProductImageUrl(
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

void DashboardPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::dashboardPageStyle(theme)
    );
}