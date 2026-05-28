#include "ScanPage.h"

#include <QDateTime>
#include <QPushButton>

ScanPage::ScanPage(
    ProductService& productService,
    ScanMode mode,
    QWidget* parent
)
    : QWidget(parent),
    productService(productService),
    mode(mode)
{
    ui.setupUi(this);

    setupPage();

    connect(ui.submitButton, &QPushButton::clicked,
        this, &ScanPage::onSubmitClicked);

    connect(ui.minusButton, &QPushButton::clicked,
        this, &ScanPage::onMinusClicked);

    connect(ui.plusButton, &QPushButton::clicked,
        this, &ScanPage::onPlusClicked);
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
    QString barcode =
        ui.barcodeInput->text().trimmed();

    int quantity =
        ui.quantityInput->value();

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

    addRecentScan(
        barcode,
        barcode,
        quantity
    );

    ui.barcodeInput->clear();
    ui.quantityInput->setValue(1);

    emit stockChanged();
}

void ScanPage::onMinusClicked()
{
    int value =
        ui.quantityInput->value();

    if (value > ui.quantityInput->minimum()) {
        ui.quantityInput->setValue(value - 1);
    }
}

void ScanPage::onPlusClicked()
{
    int value =
        ui.quantityInput->value();

    if (value < ui.quantityInput->maximum()) {
        ui.quantityInput->setValue(value + 1);
    }
}

void ScanPage::addRecentScan(
    const QString& barcode,
    const QString& productName,
    int quantity
)
{
    QString sign =
        mode == ScanMode::ScanIn ? "+" : "-";

    QString date =
        QDateTime::currentDateTime()
        .toString("MMM dd, yyyy hh:mm AP");

    ui.recentCodeLabel_3->setText(
        ui.recentCodeLabel_2->text()
    );

    ui.recentNameLabel_3->setText(
        ui.recentNameLabel_2->text()
    );

    ui.recentQtyLabel_3->setText(
        ui.recentQtyLabel_2->text()
    );

    ui.recentDateLabel_3->setText(
        ui.recentDateLabel_2->text()
    );

    ui.recentCodeLabel_2->setText(
        ui.recentCodeLabel->text()
    );

    ui.recentNameLabel_2->setText(
        ui.recentNameLabel->text()
    );

    ui.recentQtyLabel_2->setText(
        ui.recentQtyLabel->text()
    );

    ui.recentDateLabel_2->setText(
        ui.recentDateLabel->text()
    );

    ui.recentCodeLabel->setText(barcode);
    ui.recentNameLabel->setText(productName);
    ui.recentQtyLabel->setText(sign + QString::number(quantity));
    ui.recentDateLabel->setText(date);
}