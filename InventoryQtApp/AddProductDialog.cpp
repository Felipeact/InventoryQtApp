#include "AddProductDialog.h"

#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

AddProductDialog::AddProductDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    applyTheme(Theme::AppTheme::Dark);

    setWindowTitle("Add New Item");
    setModal(true);

    resize(700, 420);
    setMinimumSize(700, 420);
    setMaximumSize(700, 420);

    ui.statusLabel->setText("");

    ui.quantityInput->setMinimum(0);
    ui.quantityInput->setMaximum(999999);

    connect(
        ui.saveButton,
        &QPushButton::clicked,
        this,
        &AddProductDialog::onSaveButtonClicked
    );

    connect(
        ui.cancelButton,
        &QPushButton::clicked,
        this,
        &AddProductDialog::onCancelButtonClicked
    );

    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &AddProductDialog::onCloseButtonClicked
    );
}

AddProductDialog::~AddProductDialog()
{
}

QString AddProductDialog::getProductName() const
{
    return ui.productNameInput->text().trimmed();
}

QString AddProductDialog::getBarcode() const
{
    return ui.barcodeInput->text().trimmed();
}

int AddProductDialog::getQuantity() const
{
    return ui.quantityInput->value();
}

void AddProductDialog::onSaveButtonClicked()
{
    if (viewMode) {
        reject();
        return;
    }

    if (getProductName().isEmpty()) {
        ui.statusLabel->setText("Product name is required.");
        return;
    }

    if (getBarcode().isEmpty()) {
        ui.statusLabel->setText("Barcode is required.");
        return;
    }

    accept();
}

void AddProductDialog::onCancelButtonClicked()
{
    reject();
}

void AddProductDialog::onCloseButtonClicked()
{
    reject();
}

void AddProductDialog::setProductData(
    const QString& name,
    const QString& barcode,
    int quantity
)
{
    viewMode = false;

    ui.productNameInput->setText(name);
    ui.barcodeInput->setText(barcode);
    ui.quantityInput->setValue(quantity);

    ui.productNameInput->setReadOnly(false);
    ui.barcodeInput->setReadOnly(false);
    ui.quantityInput->setEnabled(true);

    ui.titleLabel->setText("Edit Product");
    ui.saveButton->setText("Save");
    ui.saveButton->show();
    ui.cancelButton->setText("Cancel");

    setWindowTitle("Edit Product");
}

void AddProductDialog::setViewMode(
    const QString& name,
    const QString& barcode,
    int quantity
)
{
    viewMode = true;

    ui.productNameInput->setText(name);
    ui.barcodeInput->setText(barcode);
    ui.quantityInput->setValue(quantity);

    ui.productNameInput->setReadOnly(true);
    ui.barcodeInput->setReadOnly(true);
    ui.quantityInput->setEnabled(false);

    ui.titleLabel->setText("Product Details");
    ui.saveButton->hide();
    ui.cancelButton->setText("Close");

    setWindowTitle("Product Details");
}

void AddProductDialog::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}