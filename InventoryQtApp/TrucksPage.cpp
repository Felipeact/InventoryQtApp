#include "TrucksPage.h"
#include "AddEditTruckDialog.h"

#include <QMessageBox>
#include <QHBoxLayout>
#include <QPushButton>

TrucksPage::TrucksPage(
    TruckStockService* truckStockService,
    UserService* userService,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService),
    userService(userService)
{
    ui.setupUi(this);
    setupConnections();
    loadTrucks();
}

TrucksPage::~TrucksPage()
{
}

void TrucksPage::setupConnections()
{
    connect(ui.addTruckButton, &QPushButton::clicked, this, &TrucksPage::onAddTruckClicked);
    connect(ui.searchInput, &QLineEdit::textChanged, this, &TrucksPage::onSearchChanged);
}

void TrucksPage::loadTrucks()
{
    ui.trucksTable->clearContents();

    currentTrucks = truckStockService->getTrucks();

    ui.trucksTable->setRowCount(
        static_cast<int>(currentTrucks.size())
    );

    ui.trucksTable->setColumnCount(5);

    for (int row = 0; row < static_cast<int>(currentTrucks.size()); ++row) {
        const TruckDto& truck = currentTrucks[row];

        ui.trucksTable->setItem(
            row, 0,
            new QTableWidgetItem(QString::fromStdString(truck.truckName))
        );

        ui.trucksTable->setItem(
            row, 1,
            new QTableWidgetItem(QString::fromStdString(truck.licensePlate))
        );

        ui.trucksTable->setItem(
            row, 2,
            new QTableWidgetItem(QString::fromStdString(truck.technicianName))
        );

        ui.trucksTable->setItem(
            row, 3,
            new QTableWidgetItem(QString::fromStdString(truck.status))
        );

        addActionButtons(row);
    }

    ui.paginationLabel->setText(
        QString("Showing %1 trucks").arg(currentTrucks.size())
    );
}

void TrucksPage::refreshTrucksList()
{
    loadTrucks();
}

void TrucksPage::onAddTruckClicked()
{
    AddEditTruckDialog dialog(userService, this);

    if (dialog.exec() == QDialog::Accepted) {

        CreateTruckRequest request;

        request.truckNumber = dialog.getTruckName().toStdString();
        request.plateNumber = dialog.getLicensePlate().toStdString();

        request.technicianId = dialog.getTechnicianId().toStdString();

        request.status = dialog.getStatus().toStdString();

        bool success =
            truckStockService->createTruck(request);

        if (success) {
            QMessageBox::information(
                this,
                "Success",
                "Truck created successfully."
            );

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

void TrucksPage::onSearchChanged(const QString& text)
{
    // Filter trucks based on search text
}

void TrucksPage::onPageChanged(int page)
{
    // Load trucks for specified page
}

void TrucksPage::addActionButtons(int row)
{
    QWidget* actionWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(actionWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    QPushButton* editButton = new QPushButton("Edit", actionWidget);
    QPushButton* deleteButton = new QPushButton("Delete", actionWidget);

    layout->addWidget(editButton);
    layout->addWidget(deleteButton);
    layout->addStretch();

    ui.trucksTable->setCellWidget(row, 4, actionWidget);

    connect(editButton, &QPushButton::clicked, this, [this, row]() {
        onEditTruckClicked(row);
        });

    connect(deleteButton, &QPushButton::clicked, this, [this, row]() {
        onDeleteTruckClicked(row);
        });
}

void TrucksPage::onEditTruckClicked(int row)
{
    if (row < 0 || row >= static_cast<int>(currentTrucks.size())) {
        return;
    }

    const TruckDto& truck = currentTrucks[row];

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

void TrucksPage::onDeleteTruckClicked(int row)
{
    if (row < 0 || row >= static_cast<int>(currentTrucks.size())) {
        return;
    }

    const TruckDto& truck = currentTrucks[row];

    auto confirm = QMessageBox::question(
        this,
        "Deactivate Truck",
        "Do you want to deactivate this truck?"
    );

    if (confirm != QMessageBox::Yes) {
        return;
    }

    bool success = truckStockService->deactivateTruck(truck.id);

    if (success) {
        QMessageBox::information(this, "Success", "Truck deactivated.");
        loadTrucks();
    }
    else {
        QMessageBox::warning(this, "Error", "Failed to deactivate truck.");
    }
}