#include "AddEditTemplateDialog.h"

#include <QMessageBox>
#include <QPushButton>
#include <QTableWidgetItem>

AddEditTemplateDialog::AddEditTemplateDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    setupConnections();

    ui.templateItemsTable->setRowCount(0);
}

AddEditTemplateDialog::~AddEditTemplateDialog()
{
}

void AddEditTemplateDialog::setupConnections()
{
    connect(ui.addItemButton, &QPushButton::clicked,
        this, &AddEditTemplateDialog::onAddItemClicked);

    connect(ui.saveButton, &QPushButton::clicked,
        this, &AddEditTemplateDialog::onSaveClicked);

    connect(ui.cancelButton, &QPushButton::clicked,
        this, &AddEditTemplateDialog::onCancelClicked);
}

void AddEditTemplateDialog::setEditMode(const QString& templateId)
{
    Q_UNUSED(templateId);

    ui.dialogTitle->setText("Edit Template");
}

QString AddEditTemplateDialog::getTemplateName() const
{
    return ui.templateNameInput->text().trimmed();
}

QString AddEditTemplateDialog::getDescription() const
{
    return ui.descriptionInput->toPlainText().trimmed();
}

QString AddEditTemplateDialog::getTradeType() const
{
    return ui.tradeTypeInput->text().trimmed();
}

std::vector<CreateTemplateItemRequest> AddEditTemplateDialog::getItems() const
{
    std::vector<CreateTemplateItemRequest> items;

    for (int row = 0; row < ui.templateItemsTable->rowCount(); ++row) {
        QTableWidgetItem* nameItem = ui.templateItemsTable->item(row, 0);

        if (!nameItem || nameItem->text().trimmed().isEmpty()) {
            continue;
        }

        CreateTemplateItemRequest item;

        item.productName = nameItem->text().trimmed().toStdString();

        item.category = "";

        item.requiredQuantity =
            ui.templateItemsTable->item(row, 1)
            ? ui.templateItemsTable->item(row, 1)->text().toInt()
            : 1;

        QString priceText =
            ui.templateItemsTable->item(row, 2)
            ? ui.templateItemsTable->item(row, 2)->text()
            : "0";

        priceText.replace("$", "");
        priceText = priceText.trimmed();

        item.expectedPrice = priceText.toDouble();

        item.minimumQuantity =
            ui.templateItemsTable->item(row, 3)
            ? ui.templateItemsTable->item(row, 3)->text().toInt()
            : 1;

        item.unit = "";
        item.notes = getDescription().toStdString();

        items.push_back(item);
    }

    return items;
}

void AddEditTemplateDialog::onAddItemClicked()
{
    int row = ui.templateItemsTable->rowCount();
    ui.templateItemsTable->insertRow(row);

    ui.templateItemsTable->setItem(row, 0, new QTableWidgetItem("New Item"));
    ui.templateItemsTable->setItem(row, 1, new QTableWidgetItem("1"));
    ui.templateItemsTable->setItem(row, 2, new QTableWidgetItem("0.00"));
    ui.templateItemsTable->setItem(row, 3, new QTableWidgetItem("1"));
    ui.templateItemsTable->setItem(row, 4, new QTableWidgetItem("Delete"));
}

void AddEditTemplateDialog::onSaveClicked()
{
    if (getTemplateName().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Template name is required.");
        return;
    }

    if (getItems().empty()) {
        QMessageBox::warning(this, "Validation Error", "Add at least one item.");
        return;
    }

    accept();
}

void AddEditTemplateDialog::onCancelClicked()
{
    reject();
}