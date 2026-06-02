#include "AssignTemplateDialog.h"
#include "Theme.h"

#include <QComboBox>
#include <QDate>
#include <QMessageBox>
#include <QPushButton>

AssignTemplateDialog::AssignTemplateDialog(
    TruckStockService* truckStockService,
    UserService* userService,
    QWidget* parent
)
    : QDialog(parent),
    truckStockService(truckStockService),
    userService(userService)
{
    ui.setupUi(this);

    ui.assignmentDateEdit->setDate(
        QDate::currentDate()
    );

    setupConnections();

    loadTrucks();
    loadTemplates();
    loadTechnicians();
}

AssignTemplateDialog::~AssignTemplateDialog()
{
}

void AssignTemplateDialog::setupConnections()
{
    connect(
        ui.assignButton,
        &QPushButton::clicked,
        this,
        &AssignTemplateDialog::onAssignClicked
    );

    connect(
        ui.cancelButton,
        &QPushButton::clicked,
        this,
        &AssignTemplateDialog::onCancelClicked
    );
}

void AssignTemplateDialog::loadTrucks()
{
    ui.truckComboBox->clear();

    ui.truckComboBox->addItem(
        "Select Truck",
        ""
    );

    if (!truckStockService) {
        return;
    }

    std::vector<TruckDto> trucks =
        truckStockService->getTrucks();

    for (const TruckDto& truck : trucks) {

        ui.truckComboBox->addItem(
            QString::fromStdString(truck.truckName),
            QString::fromStdString(truck.id)
        );
    }
}

void AssignTemplateDialog::loadTemplates()
{
    ui.templateComboBox->clear();

    ui.templateComboBox->addItem(
        "Select Template",
        ""
    );

    if (!truckStockService) {
        return;
    }

    std::vector<StockTemplateDto> templates =
        truckStockService->getTemplates();

    for (const StockTemplateDto& stockTemplate : templates) {

        ui.templateComboBox->addItem(
            QString::fromStdString(stockTemplate.name),
            QString::fromStdString(stockTemplate.id)
        );
    }
}

void AssignTemplateDialog::loadTechnicians()
{
    ui.technicianComboBox->clear();

    ui.technicianComboBox->addItem(
        "Select Technician",
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

        ui.technicianComboBox->addItem(
            QString::fromStdString(user.name),
            QString::fromStdString(user.id)
        );
    }
}

QString AssignTemplateDialog::getTruckId() const
{
    return ui.truckComboBox
        ->currentData()
        .toString();
}

QString AssignTemplateDialog::getTemplateId() const
{
    return ui.templateComboBox
        ->currentData()
        .toString();
}

QString AssignTemplateDialog::getTechnicianId() const
{
    return ui.technicianComboBox
        ->currentData()
        .toString();
}

QString AssignTemplateDialog::getNotes() const
{
    return ui.notesInput
        ->toPlainText()
        .trimmed();
}

QString AssignTemplateDialog::getAssignmentDate() const
{
    return ui.assignmentDateEdit
        ->date()
        .toString("yyyy-MM-dd");
}

void AssignTemplateDialog::onAssignClicked()
{
    if (getTruckId().isEmpty()) {

        QMessageBox::warning(
            this,
            "Validation Error",
            "Please select a truck."
        );

        return;
    }

    if (getTemplateId().isEmpty()) {

        QMessageBox::warning(
            this,
            "Validation Error",
            "Please select a template."
        );

        return;
    }

    if (getTechnicianId().isEmpty()) {

        QMessageBox::warning(
            this,
            "Validation Error",
            "Please select a technician."
        );

        return;
    }

    accept();
}

void AssignTemplateDialog::onCancelClicked()
{
    reject();
}

void AssignTemplateDialog::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

