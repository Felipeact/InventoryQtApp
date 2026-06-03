#include "AddAssetDialog.h"

#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

AddAssetDialog::AddAssetDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    applyTheme(Theme::AppTheme::Dark);

    setWindowTitle("Add New Asset");
    setModal(true);

    resize(700, 420);
    setMinimumSize(700, 420);
    setMaximumSize(700, 420);

    ui.errorLabel->setText("");

    connect(
        ui.saveButton,
        &QPushButton::clicked,
        this,
        &AddAssetDialog::onSaveButtonClicked
    );

    connect(
        ui.cancelButton,
        &QPushButton::clicked,
        this,
        &AddAssetDialog::onCancelButtonClicked
    );

    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &AddAssetDialog::onCloseButtonClicked
    );
}

AddAssetDialog::~AddAssetDialog()
{
}

QString AddAssetDialog::getAssetName() const
{
    return ui.assetNameInput->text().trimmed();
}

QString AddAssetDialog::getAssetType() const
{
    return ui.assetTypeInput->currentText().trimmed();
}

QString AddAssetDialog::getSerialCode() const
{
    return ui.serialCodeInput->text().trimmed();
}

QString AddAssetDialog::getStatus() const
{
    return ui.statusComboBox->currentText().trimmed();
}

QString AddAssetDialog::getDescription() const
{
    return ui.descriptionInput->text().trimmed();
}

void AddAssetDialog::onSaveButtonClicked()
{
    if (viewMode) {
        reject();
        return;
    }

    if (getAssetName().isEmpty()) {
        ui.errorLabel->setText("Asset name is required.");
        return;
    }

    if (getAssetType().isEmpty() || getAssetType() == "Select asset type") {
        ui.errorLabel->setText("Asset type is required.");
        return;
    }

    if (getSerialCode().isEmpty()) {
        ui.errorLabel->setText("Serial code is required.");
        return;
    }

    accept();
}

void AddAssetDialog::onCancelButtonClicked()
{
    reject();
}

void AddAssetDialog::onCloseButtonClicked()
{
    reject();
}

void AddAssetDialog::setAssetData(
    const QString& name,
    const QString& type,
    const QString& serialCode,
    const QString& status,
    const QString& description
)
{
    viewMode = false;

    ui.assetNameInput->setText(name);
    ui.assetTypeInput->setCurrentText(type);
    ui.serialCodeInput->setText(serialCode);
    ui.statusComboBox->setCurrentText(status);
    ui.descriptionInput->setText(description);

    ui.assetNameInput->setReadOnly(false);
    ui.assetTypeInput->setEnabled(true);
    ui.serialCodeInput->setReadOnly(false);
    ui.statusComboBox->setEnabled(true);
    ui.descriptionInput->setReadOnly(false);

    ui.titleLabel->setText("Edit Asset");
    ui.saveButton->setText("Save");
    ui.saveButton->show();
    ui.cancelButton->setText("Cancel");

    setWindowTitle("Edit Asset");
}

void AddAssetDialog::setViewMode(
    const QString& name,
    const QString& type,
    const QString& serialCode,
    const QString& status,
    const QString& description
)
{
    viewMode = true;

    ui.assetNameInput->setText(name);
    ui.assetTypeInput->setCurrentText(type);
    ui.serialCodeInput->setText(serialCode);
    ui.statusComboBox->setCurrentText(status);
    ui.descriptionInput->setText(description);

    ui.assetNameInput->setReadOnly(true);
    ui.assetTypeInput->setEnabled(false);
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