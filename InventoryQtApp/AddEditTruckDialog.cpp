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

void AddEditTruckDialog::setupConnections()
{
    connect(ui.saveButton, &QPushButton::clicked, this, &AddEditTruckDialog::onSaveClicked);
    connect(ui.cancelButton, &QPushButton::clicked, this, &AddEditTruckDialog::onCancelClicked);
}

void AddEditTruckDialog::loadTechnicianList()
{
    ui.technicianComboBox->clear();
    ui.technicianComboBox->addItem("No technician", "");

    if (!userService) {
        return;
    }

    std::vector<UserDto> technicians = userService->getTechnicians();

    for (const UserDto& tech : technicians) {
        QString displayName = QString::fromStdString(tech.name);

        if (displayName.trimmed().isEmpty()) {
            displayName = QString::fromStdString(tech.email);
        }

        ui.technicianComboBox->addItem(
            displayName,
            QString::fromStdString(tech.id)
        );
    }
}

void AddEditTruckDialog::setEditMode(const QString& truckId)
{
    // Load truck data for editing
    ui.dialogTitle->setText("Edit Truck");
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
