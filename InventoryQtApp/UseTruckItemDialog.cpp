#include "UseTruckItemDialog.h"

UseTruckItemDialog::UseTruckItemDialog(QWidget* parent)
    : QDialog(parent), maxQuantity(0)
{
    ui.setupUi(this);
    setupConnections();
    loadItems();
}

UseTruckItemDialog::~UseTruckItemDialog()
{
}

void UseTruckItemDialog::setupConnections()
{
    connect(ui.decreaseButton, &QPushButton::clicked, this, &UseTruckItemDialog::onDecreaseClicked);
    connect(ui.increaseButton, &QPushButton::clicked, this, &UseTruckItemDialog::onIncreaseClicked);
    connect(ui.useItemButton, &QPushButton::clicked, this, &UseTruckItemDialog::onUseItemClicked);
    connect(ui.cancelButton, &QPushButton::clicked, this, &UseTruckItemDialog::onCancelClicked);
    connect(ui.itemCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
        this, &UseTruckItemDialog::onItemSelected);
}

void UseTruckItemDialog::loadItems()
{
    // Load items from backend
}

void UseTruckItemDialog::setItem(const QString& itemName, int currentQty)
{
    maxQuantity = currentQty;
    ui.currentQtyValue->setText(QString::number(currentQty));
}

int UseTruckItemDialog::getQuantityToUse() const
{
    return ui.quantityDisplay->text().toInt();
}

QString UseTruckItemDialog::getNotes() const
{
    return ui.notesInput->toPlainText();
}

void UseTruckItemDialog::updateQuantityDisplay()
{
    // Update display with current quantity selection
}

void UseTruckItemDialog::onDecreaseClicked()
{
    int current = ui.quantityDisplay->text().toInt();
    if (current > 1) {
        ui.quantityDisplay->setText(QString::number(current - 1));
    }
}

void UseTruckItemDialog::onIncreaseClicked()
{
    int current = ui.quantityDisplay->text().toInt();
    if (current < maxQuantity) {
        ui.quantityDisplay->setText(QString::number(current + 1));
    }
}

void UseTruckItemDialog::onItemSelected(int index)
{
    // Update current qty when item changes
}

void UseTruckItemDialog::onUseItemClicked()
{
    // Validate and use item
    accept();
}

void UseTruckItemDialog::onCancelClicked()
{
    reject();
}
