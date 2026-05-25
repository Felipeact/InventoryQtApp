#include "TrucksPage.h"
#include "AddEditTruckDialog.h"

#include <QMessageBox>

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

        ui.trucksTable->setItem(
            row, 4,
            new QTableWidgetItem("Edit / Delete")
        );
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
