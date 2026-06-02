#include "UseTruckItemDialog.h"
#include "Theme.h"

#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QPlainTextEdit>

UseTruckItemDialog::UseTruckItemDialog(QWidget* parent)
    : QDialog(parent),
    maxQuantity(0)
{
    ui.setupUi(this);
    setupConnections();

    ui.quantityInput->setValue(1);
    ui.currentQtyValue->setText("0");
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
}

void UseTruckItemDialog::setItem(
    const QString& itemName,
    int currentQty
)
{
    maxQuantity = currentQty;

    ui.itemComboBox->clear();
    ui.itemComboBox->addItem(itemName);

    ui.currentQtyValue->setText(QString::number(currentQty));

    ui.quantityInput->setMinimum(currentQty > 0 ? 1 : 0);
    ui.quantityInput->setMaximum(currentQty);
    ui.quantityInput->setValue(currentQty > 0 ? 1 : 0);

    ui.useButton->setEnabled(currentQty > 0);
}

int UseTruckItemDialog::getQuantityToUse() const
{
    return ui.quantityInput->value();
}

QString UseTruckItemDialog::getNotes() const
{
    return ui.notesInput->toPlainText().trimmed();
}

void UseTruckItemDialog::onDecreaseClicked()
{
    int current = ui.quantityInput->value();

    if (current > ui.quantityInput->minimum()) {
        ui.quantityInput->setValue(current - 1);
    }
}

void UseTruckItemDialog::onIncreaseClicked()
{
    int current = ui.quantityInput->value();

    if (current < maxQuantity) {
        ui.quantityInput->setValue(current + 1);
    }
}

void UseTruckItemDialog::onUseItemClicked()
{
    if (getQuantityToUse() <= 0) {
        QMessageBox::warning(
            this,
            "Validation Error",
            "Quantity must be greater than 0."
        );
        return;
    }

    if (getQuantityToUse() > maxQuantity) {
        QMessageBox::warning(
            this,
            "Validation Error",
            "You cannot use more than the current quantity."
        );
        return;
    }

    accept();
}

void UseTruckItemDialog::onCancelClicked()
{
    reject();
}

void UseTruckItemDialog::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

