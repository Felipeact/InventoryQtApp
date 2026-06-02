#include "AddEditUserDialog.h"
#include "Theme.h"

#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

AddEditUserDialog::AddEditUserDialog(
    QWidget* parent
)
    : QDialog(parent)
{
    ui.setupUi(this);

    applyTheme(Theme::AppTheme::Dark);

    setupConnections();
}

AddEditUserDialog::~AddEditUserDialog()
{
}

void AddEditUserDialog::setupConnections()
{
    connect(
        ui.saveButton,
        &QPushButton::clicked,
        this,
        &AddEditUserDialog::onSaveClicked
    );

    connect(
        ui.cancelButton,
        &QPushButton::clicked,
        this,
        &AddEditUserDialog::onCancelClicked
    );
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

    int roleIndex =
        ui.roleComboBox->findText(role);

    if (roleIndex >= 0) {
        ui.roleComboBox->setCurrentIndex(roleIndex);
    }

    int statusIndex =
        ui.statusComboBox->findText(status);

    if (statusIndex >= 0) {
        ui.statusComboBox->setCurrentIndex(statusIndex);
    }

    ui.passwordInput->clear();
    ui.passwordInput->setPlaceholderText(
        "Leave blank to keep current password"
    );
}

void AddEditUserDialog::setViewMode()
{
    viewMode = true;

    setWindowTitle("View User");

    ui.nameInput->setReadOnly(true);
    ui.emailInput->setReadOnly(true);
    ui.passwordInput->setReadOnly(true);

    ui.roleComboBox->setEnabled(false);
    ui.statusComboBox->setEnabled(false);

    ui.passwordInput->clear();
    ui.passwordInput->setPlaceholderText("");

    ui.saveButton->hide();

    ui.cancelButton->setText("Close");
}

bool AddEditUserDialog::validateForm()
{
    if (viewMode) {
        return true;
    }

    if (getName().isEmpty()) {
        QMessageBox::warning(
            this,
            "Validation Error",
            "Name is required."
        );
        return false;
    }

    if (getEmail().isEmpty()) {
        QMessageBox::warning(
            this,
            "Validation Error",
            "Email is required."
        );
        return false;
    }

    if (!getEmail().contains("@")) {
        QMessageBox::warning(
            this,
            "Validation Error",
            "Please enter a valid email."
        );
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

void AddEditUserDialog::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}