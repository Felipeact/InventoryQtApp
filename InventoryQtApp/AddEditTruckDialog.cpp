#include "AddEditTruckDialog.h"

AddEditTruckDialog::AddEditTruckDialog(QWidget* parent)
    : QDialog(parent)
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
    // Load technician list from backend
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
    // Validate and save truck
    accept();
}

void AddEditTruckDialog::onCancelClicked()
{
    reject();
}
