#include "AddEditTemplateItemDialog.h"

#include <QMessageBox>
#include <QPushButton>

AddEditTemplateItemDialog::AddEditTemplateItemDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    setupConnections();
}

AddEditTemplateItemDialog::~AddEditTemplateItemDialog()
{
}

void AddEditTemplateItemDialog::setupConnections()
{
    connect(ui.saveButton, &QPushButton::clicked,
        this, &AddEditTemplateItemDialog::onSaveClicked);

    connect(ui.cancelButton, &QPushButton::clicked,
        this, &AddEditTemplateItemDialog::onCancelClicked);
}

CreateTemplateItemRequest AddEditTemplateItemDialog::getItem() const
{
    CreateTemplateItemRequest item;

    item.productName =
        ui.itemNameInput->text().trimmed().toStdString();

    item.category =
        ui.categoryInput->text().trimmed().toStdString();

    item.requiredQuantity =
        ui.requiredQtyInput->value();

    item.minimumQuantity =
        ui.minimumQtyInput->value();

    item.expectedPrice =
        ui.expectedPriceInput->value();

    item.unit =
        ui.unitInput->text().trimmed().toStdString();

    item.notes =
        ui.notesInput->toPlainText().trimmed().toStdString();

    return item;
}

void AddEditTemplateItemDialog::setItem(
    const CreateTemplateItemRequest& item
)
{
    ui.dialogTitle->setText("Edit Template Item");

    ui.itemNameInput->setText(
        QString::fromStdString(item.productName)
    );

    ui.categoryInput->setText(
        QString::fromStdString(item.category)
    );

    ui.requiredQtyInput->setValue(
        item.requiredQuantity
    );

    ui.minimumQtyInput->setValue(
        item.minimumQuantity
    );

    ui.expectedPriceInput->setValue(
        item.expectedPrice
    );

    ui.unitInput->setText(
        QString::fromStdString(item.unit)
    );

    ui.notesInput->setPlainText(
        QString::fromStdString(item.notes)
    );
}

bool AddEditTemplateItemDialog::validateForm()
{
    if (ui.itemNameInput->text().trimmed().isEmpty()) {
        QMessageBox::warning(
            this,
            "Validation Error",
            "Item name is required."
        );

        return false;
    }

    return true;
}

void AddEditTemplateItemDialog::onSaveClicked()
{
    if (!validateForm()) {
        return;
    }

    accept();
}

void AddEditTemplateItemDialog::onCancelClicked()
{
    reject();
}