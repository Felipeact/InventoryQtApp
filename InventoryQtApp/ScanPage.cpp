#include "ScanPage.h"

ScanPage::ScanPage(ProductService& productService, ScanMode mode, QWidget* parent)
    : QWidget(parent),
    productService(productService),
    mode(mode)
{
    ui.setupUi(this);

    setupPage();

    connect(
        ui.submitButton,
        &QPushButton::clicked,
        this,
        &ScanPage::onSubmitClicked
    );
}

ScanPage::~ScanPage()
{
}

void ScanPage::setupPage()
{
    ui.statusLabel->setText("");

    ui.quantityInput->setMinimum(1);
    ui.quantityInput->setMaximum(999999);

    if (mode == ScanMode::ScanIn) {
        ui.titleLabel->setText("Scan In");
        ui.submitButton->setText("Add Stock");
    }
    else {
        ui.titleLabel->setText("Scan Out");
        ui.submitButton->setText("Remove Stock");
    }
}

void ScanPage::onSubmitClicked()
{
    QString barcode = ui.barcodeInput->text().trimmed();
    int quantity = ui.quantityInput->value();

    if (barcode.isEmpty()) {
        ui.statusLabel->setText("Barcode is required.");
        return;
    }

    bool success = false;

    if (mode == ScanMode::ScanIn) {
        success = productService.scanIn(
            barcode.toStdString(),
            quantity
        );
    }
    else {
        success = productService.scanOut(
            barcode.toStdString(),
            quantity
        );
    }

    if (!success) {
        ui.statusLabel->setText("Operation failed.");
        return;
    }

    ui.statusLabel->setText("Stock updated successfully.");

    ui.barcodeInput->clear();
    ui.quantityInput->setValue(1);

    emit stockChanged();
}