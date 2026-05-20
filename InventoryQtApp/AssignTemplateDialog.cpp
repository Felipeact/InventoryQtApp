#include "AssignTemplateDialog.h"

AssignTemplateDialog::AssignTemplateDialog(QWidget* parent)
    : QDialog(parent)
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
    connect(ui.truckCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
        this, &AssignTemplateDialog::onTruckSelected);
    connect(ui.templateCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
        this, &AssignTemplateDialog::onTemplateSelected);
    connect(ui.assignButton, &QPushButton::clicked, this, &AssignTemplateDialog::onAssignClicked);
    connect(ui.cancelButton, &QPushButton::clicked, this, &AssignTemplateDialog::onCancelClicked);
}

void AssignTemplateDialog::loadTrucks()
{
    // Load trucks from backend
}

void AssignTemplateDialog::loadTemplates()
{
    // Load templates from backend
}

void AssignTemplateDialog::loadTemplatePreview(const QString& templateName)
{
    // Load and display template preview items
}

QString AssignTemplateDialog::getSelectedTruck() const
{
    return ui.truckCombo->currentText();
}

QString AssignTemplateDialog::getSelectedTemplate() const
{
    return ui.templateCombo->currentText();
}

void AssignTemplateDialog::onTruckSelected(int index)
{
    // Update display based on selected truck
}

void AssignTemplateDialog::onTemplateSelected(int index)
{
    loadTemplatePreview(ui.templateCombo->currentText());
}

void AssignTemplateDialog::onAssignClicked()
{
    // Validate and assign template to truck
    accept();
}

void AssignTemplateDialog::onCancelClicked()
{
    reject();
}
