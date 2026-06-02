// AddAssetDialog.cpp - Implementation of the asset addition dialog
#include "AddAssetDialog.h"
#include "Theme.h"

// Constructor: initializes the dialog UI and connects signals
AddAssetDialog::AddAssetDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    setWindowTitle("Add New Asset");
    setModal(true);
    resize(520, 480);
    setMinimumSize(520, 480);

    ui.statusLabel->setText("");

    connect(ui.saveButton, &QPushButton::clicked,
        this, &AddAssetDialog::onSaveButtonClicked);

    connect(ui.cancelButton, &QPushButton::clicked,
        this, &AddAssetDialog::onCancelButtonClicked);

    connect(ui.closeButton, &QPushButton::clicked,
        this, &AddAssetDialog::onCloseButtonClicked);
}

// Destructor
AddAssetDialog::~AddAssetDialog()
{
}

// Retrieves the asset name from the input field
QString AddAssetDialog::getAssetName() const
{
    return ui.assetNameInput->text().trimmed();
}

// Retrieves the asset type from the input field
QString AddAssetDialog::getAssetType() const
{
    return ui.assetTypeInput->currentText().trimmed();
}

// Retrieves the serial code from the input field
QString AddAssetDialog::getSerialCode() const
{
    return ui.serialCodeInput->text().trimmed();
}

// Retrieves the status from the combo box
QString AddAssetDialog::getStatus() const
{
    return ui.statusComboBox->currentText();
}

// Retrieves the description from the text edit
QString AddAssetDialog::getDescription() const
{
    return ui.descriptionInput->text().trimmed();
}

// Validates asset data and accepts the dialog if valid
void AddAssetDialog::onSaveButtonClicked()
{
    if (getAssetName().isEmpty()) {
        ui.statusLabel->setText("Asset name is required.");
        return;
    }

    if (getAssetType().isEmpty()) {
        ui.statusLabel->setText("Asset type is required.");
        return;
    }

    if (getSerialCode().isEmpty()) {
        ui.statusLabel->setText("Serial code is required.");
        return;
    }

    accept();
}

// Rejects the dialog without saving
void AddAssetDialog::onCancelButtonClicked()
{
    reject();
}

// Closes the dialog by rejecting it
void AddAssetDialog::onCloseButtonClicked()
{
    reject();
}

void AddAssetDialog::setAssetData(const QString& name, const QString& type, const QString& serialCode, const QString& status, const QString& description)
{
    ui.assetNameInput->setText(name);
    ui.assetTypeInput->setCurrentText(type);
    ui.serialCodeInput->setText(serialCode);
    ui.statusComboBox->setCurrentText(status);
    ui.descriptionInput->setText(description);

    setWindowTitle("Edit Asset");
    ui.titleLabel->setText("Edit Asset");
    ui.saveButton->setText("Save");
}

void AddAssetDialog::setViewMode(const QString& name, const QString& type, const QString& serialCode, const QString& status, const QString& description)
{
    ui.assetNameInput->setText(name);
    ui.assetTypeInput->setCurrentText(type);
    ui.serialCodeInput->setText(serialCode);
    ui.statusComboBox->setCurrentText(status);
    ui.descriptionInput->setText(description);

    ui.assetNameInput->setReadOnly(true);
    ui.assetTypeInput->setEnabled(true);
    ui.serialCodeInput->setReadOnly(true);
    ui.statusComboBox->setEnabled(false);
    ui.descriptionInput->setReadOnly(true);

    ui.titleLabel->setText("Asset Details");
    ui.saveButton->hide();
    ui.cancelButton->setText("Close");

    setWindowTitle("Asset Details");
}

void AddAssetDialog::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

