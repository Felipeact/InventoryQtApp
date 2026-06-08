#include "AssignTemplateDialog.h"

#include <QComboBox>
#include <QDate>
#include <QMouseEvent>
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

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);

    resize(760, 500);
    setMinimumSize(760, 500);
    setMaximumSize(760, 500);

    applyTheme(Theme::AppTheme::Dark);

    ui.errorLabel->setText("");

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

    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &AssignTemplateDialog::onCloseClicked
    );
}

void AssignTemplateDialog::setEditMode(
    const QString& truckId,
    const QString& templateId
)
{
    editMode = true;

    setWindowTitle("Edit Assignment");

    ui.titleLabel->setText("Edit Assignment");
    ui.assignButton->setText("Save Assignment");

    int truckIndex =
        ui.truckComboBox->findData(truckId);

    if (truckIndex >= 0) {
        ui.truckComboBox->setCurrentIndex(truckIndex);
    }

    int templateIndex =
        ui.templateComboBox->findData(templateId);

    if (templateIndex >= 0) {
        ui.templateComboBox->setCurrentIndex(templateIndex);
    }

    ui.technicianComboBox->setEnabled(false);
    ui.assignmentDateEdit->setEnabled(false);
    ui.notesInput->setEnabled(false);
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
    ui.errorLabel->setText("");

    if (getTruckId().isEmpty()) {
        ui.errorLabel->setText("Please select a truck.");
        return;
    }

    if (getTemplateId().isEmpty()) {
        ui.errorLabel->setText("Please select a template.");
        return;
    }

    if (!editMode && getTechnicianId().isEmpty()) {
        ui.errorLabel->setText("Please select a technician.");
        return;
    }

    accept();
}

void AssignTemplateDialog::onCancelClicked()
{
    reject();
}

void AssignTemplateDialog::onCloseClicked()
{
    reject();
}

void AssignTemplateDialog::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

void AssignTemplateDialog::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition =
            event->globalPosition().toPoint()
            - frameGeometry().topLeft();

        event->accept();
    }
}

void AssignTemplateDialog::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(
            event->globalPosition().toPoint()
            - dragPosition
        );

        event->accept();
    }
}