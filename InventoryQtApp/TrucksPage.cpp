#include "TrucksPage.h"
#include "Theme.h"
#include "AddEditTruckDialog.h"
#include "TruckDetailsDialog.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>
#include <QThread>
#include <QPointer>
#include <QMetaObject>

TrucksPage::TrucksPage(
    TruckStockService* truckStockService,
    UserService* userService,
    const std::vector<std::string>& permissions,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService),
    userService(userService),
    permissions(permissions)
{
    ui.setupUi(this);

    applyTheme(Theme::AppTheme::Dark);

    setupTable();
    setupConnections();

    // Creating/editing/deactivating trucks requires MANAGE_TRUCK_STOCK; the page
    // itself only needs VIEW_ALL_TRUCKS. Hide the create action for view-only users.
    ui.addTruckButton->setVisible(hasPermission("MANAGE_TRUCK_STOCK"));

    loadTrucks();
}

bool TrucksPage::hasPermission(
    const std::string& permission
) const
{
    return std::find(
        permissions.begin(),
        permissions.end(),
        permission
    ) != permissions.end();
}

TrucksPage::~TrucksPage()
{
}

void TrucksPage::setupConnections()
{
    connect(
        ui.addTruckButton,
        &QPushButton::clicked,
        this,
        &TrucksPage::onAddTruckClicked
    );

    connect(
        ui.searchInput,
        &QLineEdit::textChanged,
        this,
        &TrucksPage::onSearchChanged
    );

    connect(
        ui.pageButton,
        &QPushButton::clicked,
        this,
        &TrucksPage::onPreviousPageClicked
    );

    connect(
        ui.pageButton_3,
        &QPushButton::clicked,
        this,
        &TrucksPage::onNextPageClicked
    );

    connect(
        ui.pageButton_2,
        &QPushButton::clicked,
        this,
        &TrucksPage::onPage2Clicked
    );
}

void TrucksPage::setupTable()
{
    ui.trucksTable->setColumnCount(5);

    ui.trucksTable->setHorizontalHeaderLabels(
        QStringList()
        << "Truck Name"
        << "License Plate"
        << "Technician"
        << "Status"
        << "Actions"
    );

    ui.trucksTable->verticalHeader()->setVisible(false);
    ui.trucksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.trucksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.trucksTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.trucksTable->setShowGrid(false);
    ui.trucksTable->setFrameShape(QFrame::NoFrame);
    ui.trucksTable->setFocusPolicy(Qt::NoFocus);
    ui.trucksTable->setAlternatingRowColors(false);
    ui.trucksTable->viewport()->setAutoFillBackground(false);

    ui.trucksTable->horizontalHeader()->setHighlightSections(false);
    ui.trucksTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    ui.trucksTable->horizontalHeader()->setFixedHeight(48);
    ui.trucksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.trucksTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);

    ui.trucksTable->setColumnWidth(4, 130);
    ui.trucksTable->verticalHeader()->setDefaultSectionSize(52);

    ui.trucksTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.trucksTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void TrucksPage::loadTrucks()
{
    if (!truckStockService) {
        return;
    }

    QPointer<TrucksPage> self(this);
    TruckStockService* service = truckStockService;
    QThread* worker = QThread::create([self, service]() {
        std::vector<TruckDto> trucks = service->getTrucks();
        if (!self) return;
        QMetaObject::invokeMethod(self, [self, trucks]() {
            if (!self) return;
            self->currentTrucks = trucks;
            self->filterTrucks();
            }, Qt::QueuedConnection);
        });
    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void TrucksPage::refreshTrucksList()
{
    loadTrucks();
}

void TrucksPage::filterTrucks()
{
    QString searchText =
        ui.searchInput->text().trimmed();

    filteredTrucks.clear();

    for (const TruckDto& truck : currentTrucks) {
        QString truckName =
            QString::fromStdString(truck.truckName);

        QString licensePlate =
            QString::fromStdString(truck.licensePlate);

        QString technicianName =
            QString::fromStdString(truck.technicianName);

        QString status =
            QString::fromStdString(truck.status);

        bool matchesSearch =
            searchText.isEmpty() ||
            truckName.contains(searchText, Qt::CaseInsensitive) ||
            licensePlate.contains(searchText, Qt::CaseInsensitive) ||
            technicianName.contains(searchText, Qt::CaseInsensitive) ||
            status.contains(searchText, Qt::CaseInsensitive);

        if (matchesSearch) {
            filteredTrucks.push_back(truck);
        }
    }

    currentPage = 1;

    populateTable();
    updatePagination();
}

void TrucksPage::populateTable()
{
    ui.trucksTable->clearContents();

    int totalItems =
        static_cast<int>(filteredTrucks.size());

    int startIndex =
        (currentPage - 1) * pageSize;

    int endIndex =
        (std::min)(startIndex + pageSize, totalItems);

    int rowCount =
        endIndex - startIndex;

    if (rowCount == 0) {
        ui.trucksTable->setRowCount(1);

        ui.trucksTable->setItem(
            0,
            0,
            new QTableWidgetItem("No trucks found")
        );

        ui.trucksTable->setSpan(0, 0, 1, 5);

        return;
    }

    ui.trucksTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const TruckDto& truck =
            filteredTrucks[startIndex + row];

        QTableWidgetItem* truckItem =
            new QTableWidgetItem(QString::fromStdString(truck.truckName));

        truckItem->setData(
            Qt::UserRole,
            QString::fromStdString(truck.id)
        );

        ui.trucksTable->setItem(row, 0, truckItem);

        ui.trucksTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(truck.licensePlate))
        );

        ui.trucksTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::fromStdString(truck.technicianName))
        );

        ui.trucksTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::fromStdString(truck.status))
        );

        addActionButtons(row, truck.id);
    }

    ui.trucksTable->setUpdatesEnabled(true);
}

void TrucksPage::updatePagination()
{
    int totalItems =
        static_cast<int>(filteredTrucks.size());

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
        QString("Showing %1 to %2 of %3 trucks")
        .arg(startItem)
        .arg(endItem)
        .arg(totalItems)
    );

    ui.activePageButton->setText(QString::number(currentPage));
    ui.pageButton_2->setText(QString::number(currentPage + 1));

    ui.pageButton->setEnabled(currentPage > 1);
    ui.pageButton_3->setEnabled(currentPage < totalPages);
    ui.pageButton_2->setVisible(currentPage < totalPages);
}

void TrucksPage::onAddTruckClicked()
{
    AddEditTruckDialog dialog(userService, this);

    if (dialog.exec() == QDialog::Accepted) {
        CreateTruckRequest request;

        request.truckNumber =
            dialog.getTruckName().toStdString();

        request.plateNumber =
            dialog.getLicensePlate().toStdString();

        request.technicianId =
            dialog.getTechnicianId().toStdString();

        request.status =
            dialog.getStatus().toStdString();

        bool success =
            truckStockService->createTruck(request);

        if (success) {
            QMessageBox::information(
                this,
                "Success",
                "Truck created successfully."
            );

            emit trucksChanged();
            loadTrucks();
        }
        else {
            QMessageBox::warning(
                this,
                "Error",
                "Failed to create truck. Check backend console."
            );
        }
    }
}

void TrucksPage::onSearchChanged(
    const QString& text
)
{
    Q_UNUSED(text);

    filterTrucks();
}

void TrucksPage::onPreviousPageClicked()
{
    if (currentPage > 1) {
        currentPage--;

        populateTable();
        updatePagination();
    }
}

void TrucksPage::onNextPageClicked()
{
    int totalItems =
        static_cast<int>(filteredTrucks.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage < totalPages) {
        currentPage++;

        populateTable();
        updatePagination();
    }
}

void TrucksPage::onPage2Clicked()
{
    int totalItems =
        static_cast<int>(filteredTrucks.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage + 1 <= totalPages) {
        currentPage++;

        populateTable();
        updatePagination();
    }
}

void TrucksPage::addActionButtons(
    int row,
    const std::string& truckId
)
{
    QWidget* actionWidget =
        new QWidget(this);

    actionWidget->setObjectName("actionContainer");

    QHBoxLayout* layout =
        new QHBoxLayout(actionWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->setAlignment(Qt::AlignCenter);

    QPushButton* viewButton =
        new QPushButton("👁", actionWidget);

    viewButton->setObjectName("viewButton");

    QPushButton* editButton =
        new QPushButton("✎", actionWidget);

    editButton->setObjectName("editButton");

    QPushButton* deleteButton =
        new QPushButton("🗑", actionWidget);

    deleteButton->setObjectName("deleteButton");

    // Edit and deactivate both require MANAGE_TRUCK_STOCK.
    if (!hasPermission("MANAGE_TRUCK_STOCK")) {
        editButton->hide();
        deleteButton->hide();
    }

    layout->addWidget(viewButton);
    layout->addWidget(editButton);
    layout->addWidget(deleteButton);

    ui.trucksTable->setCellWidget(row, 4, actionWidget);

    connect(viewButton, &QPushButton::clicked, this, [this, truckId]() {
        onViewTruckClicked(truckId);
        });

    connect(editButton, &QPushButton::clicked, this, [this, truckId]() {
        onEditTruckClicked(truckId);
        });

    connect(deleteButton, &QPushButton::clicked, this, [this, truckId]() {
        onDeleteTruckClicked(truckId);
        });
}

void TrucksPage::onViewTruckClicked(
    const std::string& truckId
)
{
    auto it =
        std::find_if(
            currentTrucks.begin(),
            currentTrucks.end(),
            [&truckId](const TruckDto& truck) {
                return truck.id == truckId;
            }
        );

    if (it == currentTrucks.end()) {
        return;
    }

    TruckDetailsDialog dialog(
        *it,
        truckStockService,
        this
    );

    dialog.exec();
}

void TrucksPage::onEditTruckClicked(
    const std::string& truckId
)
{
    auto it =
        std::find_if(
            currentTrucks.begin(),
            currentTrucks.end(),
            [&truckId](const TruckDto& truck) {
                return truck.id == truckId;
            }
        );

    if (it == currentTrucks.end()) {
        return;
    }

    const TruckDto& truck =
        *it;

    AddEditTruckDialog dialog(userService, this);

    dialog.setEditMode(
        QString::fromStdString(truck.id),
        QString::fromStdString(truck.truckName),
        QString::fromStdString(truck.licensePlate),
        QString::fromStdString(truck.technicianId),
        QString::fromStdString(truck.status)
    );

    if (dialog.exec() == QDialog::Accepted) {
        UpdateTruckRequest request;

        request.truckNumber =
            dialog.getTruckName().toStdString();

        request.plateNumber =
            dialog.getLicensePlate().toStdString();

        request.technicianId =
            dialog.getTechnicianId().toStdString();

        request.status =
            dialog.getStatus().toStdString();

        bool success =
            truckStockService->updateTruck(truck.id, request);

        if (success) {
            QMessageBox::information(
                this,
                "Success",
                "Truck updated successfully."
            );

            emit trucksChanged();
            loadTrucks();
        }
        else {
            QMessageBox::warning(
                this,
                "Error",
                "Failed to update truck."
            );
        }
    }
}

void TrucksPage::onDeleteTruckClicked(
    const std::string& truckId
)
{
    auto it =
        std::find_if(
            currentTrucks.begin(),
            currentTrucks.end(),
            [&truckId](const TruckDto& truck) {
                return truck.id == truckId;
            }
        );

    if (it == currentTrucks.end()) {
        return;
    }

    const TruckDto& truck =
        *it;

    QMessageBox::StandardButton confirm =
        QMessageBox::question(
            this,
            "Deactivate Truck",
            "Do you want to deactivate this truck?",
            QMessageBox::Yes | QMessageBox::No
        );

    if (confirm != QMessageBox::Yes) {
        return;
    }

    bool success =
        truckStockService->deactivateTruck(truck.id);

    if (success) {
        QMessageBox::information(
            this,
            "Success",
            "Truck deactivated."
        );

        emit trucksChanged();
        loadTrucks();
    }
    else {
        QMessageBox::warning(
            this,
            "Error",
            "Failed to deactivate truck."
        );
    }
}

void TrucksPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::trucksPageStyle(theme)
    );
}

void TrucksPage::setSearchText(const QString& text)
{
    ui.searchInput->setText(text);
}