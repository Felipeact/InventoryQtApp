#include "AddEditTemplateDialog.h"

AddEditTemplateDialog::AddEditTemplateDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    setupConnections();
}

AddEditTemplateDialog::~AddEditTemplateDialog()
{
}

void AddEditTemplateDialog::setupConnections()
{
    connect(ui.addItemButton, &QPushButton::clicked, this, &AddEditTemplateDialog::onAddItemClicked);
    connect(ui.saveButton, &QPushButton::clicked, this, &AddEditTemplateDialog::onSaveClicked);
    connect(ui.cancelButton, &QPushButton::clicked, this, &AddEditTemplateDialog::onCancelClicked);
}

void AddEditTemplateDialog::setEditMode(const QString& templateId)
{
    // Load template data for editing
    ui.dialogTitle->setText("Edit Template");
}

QString AddEditTemplateDialog::getTemplateName() const
{
    return ui.templateNameInput->text();
}

QString AddEditTemplateDialog::getDescription() const
{
    return ui.descriptionInput->toPlainText();
}

void AddEditTemplateDialog::onAddItemClicked()
{
    // Open add item to template dialog
}

void AddEditTemplateDialog::onSaveClicked()
{
    // Validate and save template
    accept();
}

void AddEditTemplateDialog::onCancelClicked()
{
    reject();
}
