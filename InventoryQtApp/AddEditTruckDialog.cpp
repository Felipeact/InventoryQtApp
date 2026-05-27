#include "AddEditTruckDialog.h"
#include <QMessageBox>

AddEditTruckDialog::AddEditTruckDialog(
    UserService* userService,
    QWidget* parent
)
    : QDialog(parent),
    userService(userService)
{
    ui.setupUi(this);
    setupConnections();
    loadTechnicianList();
}

AddEditTruckDialog::~AddEditTruckDialog()
{
}

void AddEditTruckDialog::setEditMode(
    const QString& truckId,
    const QString& truckName,
    const QString& licensePlate,
    const QString& technicianId,
    const QString& status
)
{
    editingTruckId = truckId;

    ui.dialogTitle->setText("Edit Truck");

    ui.truckNameInput->setText(truckName);
    ui.licensePlateInput->setText(licensePlate);

    int technicianIndex =
        ui.technicianComboBox->findData(technicianId);

    if (technicianIndex >= 0) {
        ui.technicianComboBox->setCurrentIndex(technicianIndex);
    }

    int statusIndex =
        ui.statusComboBox->findText(status);

    if (statusIndex >= 0) {
        ui.statusComboBox->setCurrentIndex(statusIndex);
    }
}

void AddEditTruckDialog::setupConnections()
{
    connect(ui.saveButton, &QPushButton::clicked, this, &AddEditTruckDialog::onSaveClicked);
    connect(ui.cancelButton, &QPushButton::clicked, this, &AddEditTruckDialog::onCancelClicked);
}

void AddEditTruckDialog::loadTechnicianList()
{
    ui.technicianComboBox->clear();

    ui.technicianComboBox->addItem(
        "No technician",
        ""
    );

    if (!userService) {
        return;
    }

    std::vector<UserDto> users = userService->getUsers();

    for (const UserDto& user : users) {

        if (
            user.role != "TECHNICIAN" &&
            user.role != "ADMIN"
            ) {
            continue;
        }

        QString displayName =
            QString::fromStdString(user.name);

        if (displayName.trimmed().isEmpty()) {
            displayName =
                QString::fromStdString(user.email);
        }

        ui.technicianComboBox->addItem(
            displayName,
            QString::fromStdString(user.id)
        );
    }
}

QString AddEditTruckDialog::getTruckName() const
{
    return ui.truckNameInput->text();
}

QString AddEditTruckDialog::getLicensePlate() const
{
    return ui.licensePlateInput->text();
}

QString AddEditTruckDialog::getTechnician() const
{
    return ui.technicianComboBox->currentText();
}

QString AddEditTruckDialog::getTechnicianId() const
{
    return ui.technicianComboBox->currentData().toString();
}

QString AddEditTruckDialog::getStatus() const
{
    return ui.statusComboBox->currentText();
}

QString AddEditTruckDialog::getNotes() const
{
    return ui.notesInput->toPlainText();
}

void AddEditTruckDialog::onSaveClicked()
{
    if (getTruckName().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Truck name is required.");
        return;
    }

    if (getLicensePlate().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "License plate is required.");
        return;
    }

    accept();
}
void AddEditTruckDialog::onCancelClicked()
{
    reject();
}
