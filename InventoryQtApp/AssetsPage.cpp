// AssetsPage.cpp - Implementation of the assets management page
#include "AssetsPage.h"
#include "Theme.h"
#include "AssetDetailsDialog.h"

#include <algorithm>

#include <QComboBox>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QFrame>
#include <QMessageBox>
#include <QLineEdit>
#include <QThread>
#include <QPointer>
#include <QMetaObject>
#include <QInputDialog>

AssetsPage::AssetsPage(
    AssetService& assetService,
    const std::vector<std::string>& permissions,
    QWidget* parent
)
    : QWidget(parent),
    assetService(assetService),
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
    loadAssets();

    // Adding assets requires ADD_ASSET; viewing only needs VIEW_ASSET. Hide the
    // create action for users who can view but not add assets.
    ui.addAssetButton->setVisible(hasPermission("ADD_ASSET"));

    connect(
        ui.addAssetButton,
        &QPushButton::clicked,
        this,
        &AssetsPage::onAddAssetClicked
    );

    connect(
        ui.assetSearchInput,
        &QLineEdit::textChanged,
        this,
        &AssetsPage::filterAssets
    );

    connect(
        ui.filterAssetButton,
        &QPushButton::clicked,
        this,
        &AssetsPage::onFilterButtonClicked
    );

    connect(
        ui.nextPageButton,
        &QPushButton::clicked,
        this,
        &AssetsPage::onNextPageClicked
    );

    connect(
        ui.prevPageButton,
        &QPushButton::clicked,
        this,
        &AssetsPage::onPrevPageClicked
    );

    connect(
        ui.pageSizeCombo,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &AssetsPage::onPageSizeChanged
    );
}

AssetsPage::~AssetsPage()
{
}

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
    ui.assetsTable->setAlternatingRowColors(false);
    ui.assetsTable->viewport()->setAutoFillBackground(false);

    ui.assetsTable->horizontalHeader()->setHighlightSections(false);
    ui.assetsTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );
    ui.assetsTable->horizontalHeader()->setFixedHeight(48);

    ui.assetsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.assetsTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    ui.assetsTable->setColumnWidth(6, 130);

    ui.assetsTable->verticalHeader()->setDefaultSectionSize(52);

    ui.assetsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.assetsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void AssetsPage::loadAssets()
{
    QPointer<AssetsPage> self(this);
    QThread* worker = QThread::create([self]() {
        if (!self) return;
        json assets = self->assetService.getAssets();
        QMetaObject::invokeMethod(self, [self, assets]() {
            if (!self) return;
            self->currentAssets = assets;
            self->filteredAssets = self->currentAssets;
            self->currentPage = 1;
            self->populateTable(self->getCurrentPageAssets());
            self->updatePagination();
            }, Qt::QueuedConnection);
        });
    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void AssetsPage::populateTable(const json& assets)
{
    ui.assetsTable->setUpdatesEnabled(false);
    ui.assetsTable->clearContents();
    ui.assetsTable->setRowCount(static_cast<int>(assets.size()));

    // Edit/Delete are gated on EDIT_ASSET / DELETE_ASSET respectively.
    const bool canEditAssets = hasPermission("EDIT_ASSET");
    const bool canDeleteAssets = hasPermission("DELETE_ASSET");

    for (int row = 0; row < static_cast<int>(assets.size()); row++) {
        auto asset = assets[row];

        std::string assetId = asset.value("id", "");
        std::string name = asset.value("name", "");
        std::string type = asset.value("type", "");
        std::string serialCode = asset.value("serialCode", "");
        std::string status = asset.value("status", "active");
        std::string description = asset.value("description", "");
        std::string createdAt = asset.value("createdAt", "");

        std::string dateOnly =
            createdAt.length() >= 10
            ? createdAt.substr(0, 10)
            : createdAt;

        ui.assetsTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::fromStdString(name))
        );

        ui.assetsTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(type))
        );

        ui.assetsTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::fromStdString(serialCode))
        );

        ui.assetsTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::fromStdString(status))
        );

        ui.assetsTable->setItem(
            row,
            4,
            new QTableWidgetItem(QString::fromStdString(description))
        );

        ui.assetsTable->setItem(
            row,
            5,
            new QTableWidgetItem(QString::fromStdString(dateOnly))
        );

        QWidget* actionWidget = new QWidget();
        actionWidget->setObjectName("actionContainer");

        QHBoxLayout* actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 0, 0);
        actionLayout->setSpacing(6);
        actionLayout->setAlignment(Qt::AlignCenter);

        QPushButton* viewButton = new QPushButton("👁");
        viewButton->setObjectName("viewButton");

        QPushButton* editButton = new QPushButton("✎");
        editButton->setObjectName("editButton");

        QPushButton* deleteButton = new QPushButton("🗑");
        deleteButton->setObjectName("deleteButton");

        connect(viewButton, &QPushButton::clicked, this, [this, asset]() {
            AssetDetailsDialog dialog(
                asset,
                this
            );

            dialog.exec();
            });

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

        if (!canEditAssets) {
            editButton->hide();
        }

        if (!canDeleteAssets) {
            deleteButton->hide();
        }

        actionLayout->addWidget(viewButton);
        actionLayout->addWidget(editButton);
        actionLayout->addWidget(deleteButton);

        ui.assetsTable->setCellWidget(row, 6, actionWidget);
    }

    ui.assetsTable->setUpdatesEnabled(true);
}

void AssetsPage::deleteAsset(const std::string& assetId)
{
    assetService.deleteAsset(assetId);
}

void AssetsPage::filterAssets(const QString& searchText)
{
    if (searchText.trimmed().isEmpty()) {
        filteredAssets = currentAssets;
    }
    else {
        filteredAssets = json::array();
        const QString query = searchText.trimmed();

        for (const auto& asset : currentAssets) {
            QString name = QString::fromStdString(asset.value("name", ""));
            QString type = QString::fromStdString(asset.value("type", ""));
            QString serialCode = QString::fromStdString(asset.value("serialCode", ""));
            QString status = QString::fromStdString(asset.value("status", ""));

            bool matchesSearch =
                name.contains(query, Qt::CaseInsensitive) ||
                type.contains(query, Qt::CaseInsensitive) ||
                serialCode.contains(query, Qt::CaseInsensitive);

            bool matchesStatus =
                currentStatusFilter.isEmpty() ||
                currentStatusFilter == "All" ||
                status.compare(
                    currentStatusFilter,
                    Qt::CaseInsensitive
                ) == 0;

            if (matchesSearch && matchesStatus)
            {
                filteredAssets.push_back(asset);
            }
        }
    }

    currentPage = 1;

    populateTable(getCurrentPageAssets());
    updatePagination();
}

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

void AssetsPage::setSearchText(const QString& text)
{
    ui.assetSearchInput->setText(text);
}

void AssetsPage::updatePagination()
{
    int totalItems = static_cast<int>(filteredAssets.size());
    int totalPages = (std::max)(1, (totalItems + pageSize - 1) / pageSize);

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
    QStringList options;

    options
        << "All"
        << "Active"
        << "Inactive"
        << "Maintenance";

    bool ok = false;

    QString selected =
        QInputDialog::getItem(
            this,
            "Filter Assets",
            "Status:",
            options,
            0,
            false,
            &ok
        );

    if (!ok)
        return;

    currentStatusFilter = selected;

    filterAssets(
        ui.assetSearchInput->text()
    );
}

void AssetsPage::onNextPageClicked()
{
    int totalItems = static_cast<int>(filteredAssets.size());
    int totalPages = (std::max)(1, (totalItems + pageSize - 1) / pageSize);

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

bool AssetsPage::hasPermission(
    const std::string& permission
) const
{
    return std::find(
        permissions.begin(),
        permissions.end(),
        permission
    ) != permissions.end();
}

void AssetsPage::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::assetsPageStyle(theme)
    );
}