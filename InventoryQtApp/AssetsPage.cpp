// AssetsPage.cpp - Implementation of the assets management page
#include "AssetsPage.h"
#include "Theme.h"
#include <algorithm>

#include <QComboBox>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QWidget>
#include <QFrame>
#include <QMessageBox>

// Constructor initializes the assets page with table setup and asset data
AssetsPage::AssetsPage(AssetService& assetService, QWidget* parent)
    : QWidget(parent), assetService(assetService)
{
    ui.setupUi(this);
    ui.pageSizeCombo->clear();
    ui.pageSizeCombo->addItem("5 / page");
    ui.pageSizeCombo->addItem("10 / page");
    ui.pageSizeCombo->addItem("20 / page");
    ui.pageSizeCombo->setCurrentText("10 / page");

    setupTable();
    loadAssets();

    connect(ui.addAssetButton, &QPushButton::clicked,
        this, &AssetsPage::onAddAssetClicked);

    connect(ui.assetSearchInput, &QLineEdit::textChanged,
        this, &AssetsPage::filterAssets);

    connect(ui.filterAssetButton, &QPushButton::clicked,
        this, &AssetsPage::onFilterButtonClicked);

    connect(ui.nextPageButton, &QPushButton::clicked,
        this, &AssetsPage::onNextPageClicked);

    connect(ui.prevPageButton, &QPushButton::clicked,
        this, &AssetsPage::onPrevPageClicked);

    connect(ui.pageSizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &AssetsPage::onPageSizeChanged);
}

// Destructor
AssetsPage::~AssetsPage()
{
}

// Configures the assets table columns, headers, styling, and behavior
void AssetsPage::setupTable()
{
    ui.assetsTable->setColumnCount(7);

    ui.assetsTable->setHorizontalHeaderLabels(
        QStringList()
        << "Asset Name"
        << "Type"
        << "Serial Code"
        << "Status"
        << "Description"
        << "Created"
        << "Action"
    );

    ui.assetsTable->verticalHeader()->setVisible(false);
    ui.assetsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.assetsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.assetsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.assetsTable->setShowGrid(false);
    ui.assetsTable->setFrameShape(QFrame::NoFrame);
    ui.assetsTable->setFocusPolicy(Qt::NoFocus);

    ui.assetsTable->horizontalHeader()->setHighlightSections(false);
    ui.assetsTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui.assetsTable->horizontalHeader()->setFixedHeight(48);

    ui.assetsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.assetsTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    ui.assetsTable->setColumnWidth(6, 130);

    ui.assetsTable->verticalHeader()->setDefaultSectionSize(52);

    ui.assetsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.assetsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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

    ui.assetsTable->verticalScrollBar()->setStyleSheet(premiumScrollBarStyle);
    ui.assetsTable->horizontalScrollBar()->setStyleSheet(premiumScrollBarStyle);

}

// Populates the table with asset data and action buttons
void AssetsPage::loadAssets()
{
    currentAssets = assetService.getAssets();
    filteredAssets = currentAssets;

    currentPage = 1;

    populateTable(getCurrentPageAssets());
    updatePagination();
}

void AssetsPage::populateTable(const json& assets)
{
    ui.assetsTable->setRowCount(static_cast<int>(assets.size()));

    for (int row = 0; row < assets.size(); row++) {
        auto asset = assets[row];
        std::string assetId = asset.value("id", "");

        std::string name = asset.value("name", "");
        std::string type = asset.value("type", "");
        std::string serialCode = asset.value("serialCode", "");
        std::string status = asset.value("status", "active");
        std::string description = asset.value("description", "");
        std::string createdAt = asset.value("createdAt", "");

        // Extract just the date from the createdAt timestamp
        std::string dateOnly = createdAt.substr(0, 10);

        ui.assetsTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(name)));
        ui.assetsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(type)));
        ui.assetsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(serialCode)));
        ui.assetsTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(status)));
        ui.assetsTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(description)));
        ui.assetsTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(dateOnly)));

        QWidget* actionWidget = new QWidget();
        QHBoxLayout* actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 0, 0);
        actionLayout->setSpacing(6);

        QPushButton* viewButton = new QPushButton("👁");
        connect(viewButton, &QPushButton::clicked, this, [this, asset]() {
            std::string name = asset.value("name", "");
            std::string type = asset.value("type", "");
            std::string serialCode = asset.value("serialCode", "");
            std::string status = asset.value("status", "active");
            std::string description = asset.value("description", "");

            AddAssetDialog dialog(this);
            dialog.setViewMode(
                QString::fromStdString(name),
                QString::fromStdString(type),
                QString::fromStdString(serialCode),
                QString::fromStdString(status),
                QString::fromStdString(description)
            );

            dialog.exec();
            });


        QPushButton* editButton = new QPushButton("✎");
        connect(editButton, &QPushButton::clicked, this, [this, assetId, asset]() {
            std::string name = asset.value("name", "");
            std::string type = asset.value("type", "");
            std::string serialCode = asset.value("serialCode", "");
            std::string status = asset.value("status", "active");
            std::string description = asset.value("description", "");

            AddAssetDialog dialog(this);
            dialog.setAssetData(
                QString::fromStdString(name),
                QString::fromStdString(type),
                QString::fromStdString(serialCode),
                QString::fromStdString(status),
                QString::fromStdString(description)
            );

            if (dialog.exec() == QDialog::Accepted) {
                bool success = assetService.updateAsset(
                    assetId,
                    dialog.getAssetName().toStdString(),
                    dialog.getAssetType().toStdString(),
                    dialog.getSerialCode().toStdString(),
                    dialog.getStatus().toStdString(),
                    dialog.getDescription().toStdString()
                );

                if (success) {
                    currentAssets = assetService.getAssets(true);
                    filterAssets(ui.assetSearchInput->text());
                    emit assetsChanged();
                }
            }
            });


        QPushButton* deleteButton = new QPushButton("🗑");
        connect(deleteButton, &QPushButton::clicked, this, [this, assetId]() {
            QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                "Delete Asset",
                "Are you sure you want to delete this asset?",
                QMessageBox::Yes | QMessageBox::No
            );

            if (reply != QMessageBox::Yes) {
                return;
            }

            bool success = assetService.deleteAsset(assetId);

            if (success) {
                currentAssets = assetService.getAssets(true);
                filterAssets(ui.assetSearchInput->text());
                emit assetsChanged();

                QMessageBox::information(
                    this,
                    "Deleted",
                    "Asset deleted successfully."
                );
            }
            else {
                QMessageBox::warning(
                    this,
                    "Error",
                    "Could not delete asset."
                );
            }
            });

        viewButton->setFixedSize(26, 26);
        editButton->setFixedSize(26, 26);
        deleteButton->setFixedSize(26, 26);

        actionLayout->addWidget(viewButton);
        actionLayout->addWidget(editButton);
        actionLayout->addWidget(deleteButton);
        actionLayout->addStretch();

        ui.assetsTable->setCellWidget(row, 6, actionWidget);
    }
}

void AssetsPage::deleteAsset(const std::string& assetId)
{
    auto res = assetService.deleteAsset(assetId);
}

void AssetsPage::filterAssets(const QString& searchText)
{
    if (searchText.trimmed().isEmpty()) {
        filteredAssets = currentAssets;
    }
    else {
        filteredAssets = assetService.searchAssets(searchText.toStdString());
    }

    currentPage = 1;

    populateTable(getCurrentPageAssets());
    updatePagination();
}
// Opens the add asset dialog and adds new items to the table
void AssetsPage::onAddAssetClicked()
{
    AddAssetDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        bool success = assetService.createAsset(
            dialog.getAssetName().toStdString(),
            dialog.getAssetType().toStdString(),
            dialog.getSerialCode().toStdString(),
            dialog.getStatus().toStdString(),
            dialog.getDescription().toStdString()
        );

        if (success) {
            currentAssets = assetService.getAssets(true);

            filterAssets(ui.assetSearchInput->text());

            emit assetsChanged();
        }
    }
}

json AssetsPage::getCurrentPageAssets() const
{
    json pageAssets = json::array();

    int totalItems = static_cast<int>(filteredAssets.size());

    int startIndex = (currentPage - 1) * pageSize;
    int endIndex = (std::min)(startIndex + pageSize, totalItems);

    for (int i = startIndex; i < endIndex; i++) {
        pageAssets.push_back(filteredAssets[i]);
    }

    return pageAssets;
}

void AssetsPage::updatePagination()
{
    int totalItems = static_cast<int>(filteredAssets.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage > totalPages) {
        currentPage = totalPages;
    }

    int startItem =
        totalItems == 0 ? 0 : ((currentPage - 1) * pageSize) + 1;

    int endItem =
        (std::min)(currentPage * pageSize, totalItems);

    ui.paginationLabel->setText(
        QString("Showing %1 to %2 of %3 assets")
        .arg(startItem)
        .arg(endItem)
        .arg(totalItems)
    );

    ui.activePageButton->setText(QString::number(currentPage));

    ui.page2Button->setText(QString::number(currentPage + 1));

    ui.prevPageButton->setEnabled(currentPage > 1);
    ui.nextPageButton->setEnabled(currentPage < totalPages);

    ui.page2Button->setVisible(currentPage < totalPages);
}

void AssetsPage::onFilterButtonClicked()
{
    filterAssets(ui.assetSearchInput->text());
}

void AssetsPage::onNextPageClicked()
{
    int totalItems = static_cast<int>(filteredAssets.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage < totalPages) {
        currentPage++;

        populateTable(getCurrentPageAssets());
        updatePagination();
    }
}

void AssetsPage::onPrevPageClicked()
{
    if (currentPage > 1) {
        currentPage--;

        populateTable(getCurrentPageAssets());
        updatePagination();
    }
}

void AssetsPage::onPageSizeChanged(int index)
{
    QString text = ui.pageSizeCombo->itemText(index);

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

    populateTable(getCurrentPageAssets());
    updatePagination();
}

void AssetsPage::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::dataPageStyle(theme)
    );
}

