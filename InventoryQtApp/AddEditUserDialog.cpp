#include "AddEditUserDialog.h"

#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QMouseEvent>

AddEditUserDialog::AddEditUserDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);

    resize(700, 420);
    setMinimumSize(700, 420);
    setMaximumSize(700, 420);

    applyTheme(Theme::AppTheme::Dark);

    ui.errorLabel->setText("");

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

    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &AddEditUserDialog::onCloseClicked
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
    viewMode = false;

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

    ui.nameInput->setReadOnly(false);
    ui.emailInput->setReadOnly(false);
    ui.passwordInput->setReadOnly(false);
    ui.roleComboBox->setEnabled(true);
    ui.statusComboBox->setEnabled(true);

    ui.titleLabel->setText("Edit User");
    ui.saveButton->setText("Save");
    ui.saveButton->show();
    ui.cancelButton->setText("Cancel");

    setWindowTitle("Edit User");
}

void AddEditUserDialog::setViewMode()
{
    viewMode = true;

    setWindowTitle("User Details");

    ui.nameInput->setReadOnly(true);
    ui.emailInput->setReadOnly(true);
    ui.passwordInput->setReadOnly(true);

    ui.roleComboBox->setEnabled(false);
    ui.statusComboBox->setEnabled(false);

    ui.passwordInput->clear();
    ui.passwordInput->setPlaceholderText("");

    ui.titleLabel->setText("User Details");
    ui.saveButton->hide();
    ui.cancelButton->setText("Close");
}

bool AddEditUserDialog::validateForm()
{
    ui.errorLabel->setText("");

    if (viewMode) {
        return true;
    }

    if (getName().isEmpty()) {
        ui.errorLabel->setText("Name is required.");
        return false;
    }

    if (getEmail().isEmpty()) {
        ui.errorLabel->setText("Email is required.");
        return false;
    }

    if (!getEmail().contains("@")) {
        ui.errorLabel->setText("Please enter a valid email.");
        return false;
    }

    return true;
}

void AddEditUserDialog::setInviteMode()
{
    viewMode = false;

    setWindowTitle("Invite User");

    ui.titleLabel->setText("Invite User");
    ui.saveButton->setText("Invite User");

    ui.passwordLabel->hide();
    ui.passwordInput->hide();

    ui.statusComboBox->setCurrentText("Active");
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

void AddEditUserDialog::onCloseClicked()
{
    reject();
}

void AddEditUserDialog::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

void AddEditUserDialog::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition =
            event->globalPosition().toPoint()
            - frameGeometry().topLeft();

        event->accept();
    }
}

void AddEditUserDialog::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(
            event->globalPosition().toPoint()
            - dragPosition
        );

        event->accept();
    }
}