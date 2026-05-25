#include "AddEditUserDialog.h"

#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

AddEditUserDialog::AddEditUserDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    setupConnections();
}

AddEditUserDialog::~AddEditUserDialog()
{
}

void AddEditUserDialog::setupConnections()
{
    connect(ui.saveButton, &QPushButton::clicked,
        this, &AddEditUserDialog::onSaveClicked);

    connect(ui.cancelButton, &QPushButton::clicked,
        this, &AddEditUserDialog::onCancelClicked);
}

QString AddEditUserDialog::getName() const
{
    return ui.nameInput->text().trimmed();
}

QString AddEditUserDialog::getEmail() const
{
    return ui.emailInput->text().trimmed();
}

QString AddEditUserDialog::getPassword() const
{
    return ui.passwordInput->text();
}

QString AddEditUserDialog::getRole() const
{
    return ui.roleComboBox->currentText();
}

QString AddEditUserDialog::getStatus() const
{
    return ui.statusComboBox->currentText();
}

void AddEditUserDialog::setUser(
    const QString& name,
    const QString& email,
    const QString& role,
    const QString& status
)
{
    ui.nameInput->setText(name);
    ui.emailInput->setText(email);

    int roleIndex = ui.roleComboBox->findText(role);
    if (roleIndex >= 0) {
        ui.roleComboBox->setCurrentIndex(roleIndex);
    }

    int statusIndex = ui.statusComboBox->findText(status);
    if (statusIndex >= 0) {
        ui.statusComboBox->setCurrentIndex(statusIndex);
    }

    ui.passwordInput->clear();
    ui.passwordInput->setPlaceholderText("Leave blank to keep current password");
}

bool AddEditUserDialog::validateForm()
{
    if (getName().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Name is required.");
        return false;
    }

    if (getEmail().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Email is required.");
        return false;
    }

    if (!getEmail().contains("@")) {
        QMessageBox::warning(this, "Validation Error", "Please enter a valid email.");
        return false;
    }

    return true;
}

void AddEditUserDialog::onSaveClicked()
{
    if (!validateForm()) {
        return;
    }

    accept();
}

void AddEditUserDialog::onCancelClicked()
{
    reject();
}