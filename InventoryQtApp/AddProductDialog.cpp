// AddProductDialog.cpp - Implementation of the product addition dialog
#include "AddProductDialog.h"

// Constructor: initializes the dialog UI and connects signals
AddProductDialog::AddProductDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Add New Item");
	setModal(true);
	resize(520, 420);
	setMinimumSize(520, 420);

	ui.statusLabel->setText("");

	ui.quantityInput->setMinimum(0);
	ui.quantityInput->setMaximum(999999);

	connect(ui.saveButton, &QPushButton::clicked,
		this, &AddProductDialog::onSaveButtonClicked	);

	connect(ui.cancelButton, &QPushButton::clicked,
		this, &AddProductDialog::onCancelButtonClicked);

	connect(ui.closeButton, &QPushButton::clicked,
		this, &AddProductDialog::onCloseButtonClicked);

}

// Destructor
AddProductDialog::~AddProductDialog()
{}

// Retrieves the product name from the input field
QString AddProductDialog::getProductName() const
{
	return ui.productNameInput->text().trimmed();
}

// Retrieves the barcode from the input field
QString AddProductDialog::getBarcode() const
{
	return ui.barcodeInput->text().trimmed();
}

// Retrieves the quantity from the spin box
int AddProductDialog::getQuantity() const
{
	return ui.quantityInput->value();
}

// Validates product data and accepts the dialog if valid
void AddProductDialog::onSaveButtonClicked()
{
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

// Rejects the dialog without saving
void AddProductDialog::onCancelButtonClicked()
{
	reject();
}

// Closes the dialog by rejecting it
void AddProductDialog::onCloseButtonClicked()
{
	reject();
}

void AddProductDialog::setProductData(const QString& name, const QString& barcode, int quantity)
{
	ui.productNameInput->setText(name);
	ui.barcodeInput->setText(barcode);
	ui.quantityInput->setValue(quantity);

	setWindowTitle("Edit Product");
	ui.titleLabel->setText("Edit Product");
	ui.saveButton->setText("Save");
}

void AddProductDialog::setViewMode(const QString& name, const QString& barcode, int quantity)
{
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
