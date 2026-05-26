#include "AssignTemplateDialog.h"

#include <QComboBox>
#include <QMessageBox>
#include <QPushButton>

AssignTemplateDialog::AssignTemplateDialog(
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QDialog(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);
    setupConnections();
    loadTrucks();
    loadTemplates();
}

AssignTemplateDialog::~AssignTemplateDialog()
{
}

void AssignTemplateDialog::setupConnections()
{
    connect(ui.assignButton, &QPushButton::clicked,
        this, &AssignTemplateDialog::onAssignClicked);

    connect(ui.cancelButton, &QPushButton::clicked,
        this, &AssignTemplateDialog::onCancelClicked);
}

void AssignTemplateDialog::loadTrucks()
{
    ui.truckComboBox->clear();
    ui.truckComboBox->addItem("Select Truck", "");

    std::vector<TruckDto> trucks = truckStockService->getTrucks();

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
    ui.templateComboBox->addItem("Select Template", "");

    std::vector<StockTemplateDto> templates =
        truckStockService->getTemplates();

    for (const StockTemplateDto& stockTemplate : templates) {
        ui.templateComboBox->addItem(
            QString::fromStdString(stockTemplate.name),
            QString::fromStdString(stockTemplate.id)
        );
    }
}

QString AssignTemplateDialog::getTruckId() const
{
    return ui.truckComboBox->currentData().toString();
}

QString AssignTemplateDialog::getTemplateId() const
{
    return ui.templateComboBox->currentData().toString();
}

void AssignTemplateDialog::onAssignClicked()
{
    if (getTruckId().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please select a truck.");
        return;
    }

    if (getTemplateId().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please select a template.");
        return;
    }

    accept();
}

void AssignTemplateDialog::onCancelClicked()
{
    reject();
}