#include "UseTruckItemDialog.h"

#include <QMessageBox>

UseTruckItemDialog::UseTruckItemDialog(QWidget* parent)
    : QDialog(parent), maxQuantity(0)
{
    ui.setupUi(this);
    setupConnections();
    loadItems();

    ui.quantityInput->setMinimum(1);
    ui.quantityInput->setValue(1);
}

UseTruckItemDialog::~UseTruckItemDialog()
{
}

void UseTruckItemDialog::setupConnections()
{
    connect(ui.minusButton, &QPushButton::clicked,
        this, &UseTruckItemDialog::onDecreaseClicked);

    connect(ui.plusButton, &QPushButton::clicked,
        this, &UseTruckItemDialog::onIncreaseClicked);

    connect(ui.useButton, &QPushButton::clicked,
        this, &UseTruckItemDialog::onUseItemClicked);

    connect(ui.cancelButton, &QPushButton::clicked,
        this, &UseTruckItemDialog::onCancelClicked);

    connect(ui.itemComboBox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &UseTruckItemDialog::onItemSelected);
}

void UseTruckItemDialog::loadItems()
{
    // Later we will load items from backend.
    // For now this keeps the dialog safe.
}

void UseTruckItemDialog::setItem(const QString& itemName, int currentQty)
{
    maxQuantity = currentQty;

    ui.currentQtyValue->setText(QString::number(currentQty));

    ui.quantityInput->setMinimum(1);
    ui.quantityInput->setMaximum(currentQty > 0 ? currentQty : 1);
    ui.quantityInput->setValue(currentQty > 0 ? 1 : 0);

    int index = ui.itemComboBox->findText(itemName);

    if (index >= 0) {
        ui.itemComboBox->setCurrentIndex(index);
    }
}

int UseTruckItemDialog::getQuantityToUse() const
{
    return ui.quantityInput->value();
}

QString UseTruckItemDialog::getNotes() const
{
    return ui.notesInput->toPlainText();
}

void UseTruckItemDialog::updateQuantityDisplay()
{
    if (maxQuantity > 0 && ui.quantityInput->value() > maxQuantity) {
        ui.quantityInput->setValue(maxQuantity);
    }
}

void UseTruckItemDialog::onDecreaseClicked()
{
    int current = ui.quantityInput->value();

    if (current > 1) {
        ui.quantityInput->setValue(current - 1);
    }
}

void UseTruckItemDialog::onIncreaseClicked()
{
    int current = ui.quantityInput->value();

    if (maxQuantity <= 0 || current < maxQuantity) {
        ui.quantityInput->setValue(current + 1);
    }
}

void UseTruckItemDialog::onItemSelected(int index)
{
    Q_UNUSED(index);

    // Later we will update current quantity from selected backend item.
}

void UseTruckItemDialog::onUseItemClicked()
{
    if (maxQuantity <= 0) {
        QMessageBox::warning(this, "Invalid Quantity", "This item has no stock available.");
        return;
    }

    if (ui.quantityInput->value() > maxQuantity) {
        QMessageBox::warning(this, "Invalid Quantity", "Quantity cannot be greater than current stock.");
        return;
    }

    accept();
}

void UseTruckItemDialog::onCancelClicked()
{
    reject();
}