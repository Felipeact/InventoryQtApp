#include "AddEditTruckDialog.h"

#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QMouseEvent>

AddEditTruckDialog::AddEditTruckDialog(
    UserService* userService,
    QWidget* parent
)
    : QDialog(parent),
    userService(userService)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);

    resize(760, 480);
    setMinimumSize(760, 480);
    setMaximumSize(760, 480);

    applyTheme(Theme::AppTheme::Dark);

    ui.errorLabel->setText("");

    setupConnections();
    loadTechnicianList();
}

AddEditTruckDialog::~AddEditTruckDialog()
{
}

void AddEditTruckDialog::setupConnections()
{
    connect(
        ui.saveButton,
        &QPushButton::clicked,
        this,
        &AddEditTruckDialog::onSaveClicked
    );

    connect(
        ui.cancelButton,
        &QPushButton::clicked,
        this,
        &AddEditTruckDialog::onCancelClicked
    );

    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &AddEditTruckDialog::onCloseClicked
    );
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

    std::vector<UserDto> users =
        userService->getUsers();

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

void AddEditTruckDialog::setEditMode(
    const QString& truckId,
    const QString& truckName,
    const QString& licensePlate,
    const QString& technicianId,
    const QString& status
)
{
    editingTruckId = truckId;

    ui.titleLabel->setText("Edit Truck");
    setWindowTitle("Edit Truck");

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

    ui.saveButton->setText("Save Truck");
}

QString AddEditTruckDialog::getTruckName() const
{
    return ui.truckNameInput->text().trimmed();
}

QString AddEditTruckDialog::getLicensePlate() const
{
    return ui.licensePlateInput->text().trimmed();
}

QString AddEditTruckDialog::getTechnician() const
{
    return ui.technicianComboBox->currentText().trimmed();
}

QString AddEditTruckDialog::getTechnicianId() const
{
    return ui.technicianComboBox->currentData().toString();
}

QString AddEditTruckDialog::getStatus() const
{
    return ui.statusComboBox->currentText().trimmed();
}

QString AddEditTruckDialog::getNotes() const
{
    return ui.notesInput->toPlainText().trimmed();
}

void AddEditTruckDialog::onSaveClicked()
{
    ui.errorLabel->setText("");

    if (getTruckName().isEmpty()) {
        ui.errorLabel->setText("Truck name is required.");
        return;
    }

    if (getLicensePlate().isEmpty()) {
        ui.errorLabel->setText("License plate is required.");
        return;
    }

    accept();
}

void AddEditTruckDialog::onCancelClicked()
{
    reject();
}

void AddEditTruckDialog::onCloseClicked()
{
    reject();
}

void AddEditTruckDialog::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

void AddEditTruckDialog::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition =
            event->globalPosition().toPoint()
            - frameGeometry().topLeft();

        event->accept();
    }
}

void AddEditTruckDialog::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(
            event->globalPosition().toPoint()
            - dragPosition
        );

        event->accept();
    }
}