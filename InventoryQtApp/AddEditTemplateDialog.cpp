#include "AddEditTemplateDialog.h"
#include "Theme.h"
#include "AddEditTemplateItemDialog.h"


#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>
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
    AddEditTemplateItemDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        addItemRow(dialog.getItem());
    }
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
void AddEditTemplateDialog::addItemRow(const CreateTemplateItemRequest& item)
{
    int row = ui.templateItemsTable->rowCount();
    ui.templateItemsTable->insertRow(row);

    ui.templateItemsTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(item.productName)));
    ui.templateItemsTable->setItem(row, 1, new QTableWidgetItem(QString::number(item.requiredQuantity)));
    ui.templateItemsTable->setItem(row, 2, new QTableWidgetItem(QString::number(item.expectedPrice, 'f', 2)));
    ui.templateItemsTable->setItem(row, 3, new QTableWidgetItem(QString::number(item.minimumQuantity)));

    addItemActionButtons(row);
}

CreateTemplateItemRequest AddEditTemplateDialog::getItemFromRow(int row) const
{
    CreateTemplateItemRequest item;

    item.productName = ui.templateItemsTable->item(row, 0)->text().toStdString();
    item.requiredQuantity = ui.templateItemsTable->item(row, 1)->text().toInt();
    item.expectedPrice = ui.templateItemsTable->item(row, 2)->text().toDouble();
    item.minimumQuantity = ui.templateItemsTable->item(row, 3)->text().toInt();

    return item;
}

void AddEditTemplateDialog::addItemActionButtons(int row)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);

    layout->setContentsMargins(0, 0, 0, 0);

    QPushButton* editButton = new QPushButton("Edit", widget);
    QPushButton* deleteButton = new QPushButton("Delete", widget);

    layout->addWidget(editButton);
    layout->addWidget(deleteButton);

    ui.templateItemsTable->setCellWidget(row, 4, widget);

    connect(editButton, &QPushButton::clicked, this, [this, row]() {
        onEditItemClicked(row);
        });

    connect(deleteButton, &QPushButton::clicked, this, [this, row]() {
        onDeleteItemClicked(row);
        });
}

void AddEditTemplateDialog::onEditItemClicked(int row)
{
    AddEditTemplateItemDialog dialog(this);
    dialog.setItem(getItemFromRow(row));

    if (dialog.exec() == QDialog::Accepted) {
        CreateTemplateItemRequest item = dialog.getItem();

        ui.templateItemsTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(item.productName)));
        ui.templateItemsTable->setItem(row, 1, new QTableWidgetItem(QString::number(item.requiredQuantity)));
        ui.templateItemsTable->setItem(row, 2, new QTableWidgetItem(QString::number(item.expectedPrice, 'f', 2)));
        ui.templateItemsTable->setItem(row, 3, new QTableWidgetItem(QString::number(item.minimumQuantity)));

        addItemActionButtons(row);
    }
}

void AddEditTemplateDialog::onDeleteItemClicked(int row)
{
    ui.templateItemsTable->removeRow(row);
}

void AddEditTemplateDialog::setTemplateData(
    const TemplateDetailsDto& templateDetails,
    bool readOnly
)
{
    readOnlyMode = readOnly;

    ui.dialogTitle->setText(
        readOnly ? "View Template" : "Edit Template"
    );

    ui.templateNameInput->setText(
        QString::fromStdString(templateDetails.name)
    );

    ui.tradeTypeInput->setText(
        QString::fromStdString(templateDetails.tradeType)
    );

    ui.templateItemsTable->setRowCount(0);

    for (const auto& item : templateDetails.items) {
        addItemRow(item);
    }

    ui.templateNameInput->setReadOnly(readOnly);
    ui.tradeTypeInput->setReadOnly(readOnly);
    ui.descriptionInput->setReadOnly(readOnly);

    ui.addItemButton->setVisible(!readOnly);
    ui.saveButton->setVisible(!readOnly);
}

void AddEditTemplateDialog::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

