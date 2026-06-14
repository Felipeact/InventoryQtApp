#include "ScanPage.h"
#include "AsyncTask.h"

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

    applyTheme(Theme::AppTheme::Dark);

    setupPage();

    connect(
        ui.submitButton,
        &QPushButton::clicked,
        this,
        &ScanPage::onSubmitClicked
    );

    connect(
        ui.minusButton,
        &QPushButton::clicked,
        this,
        &ScanPage::onMinusClicked
    );

    connect(
        ui.plusButton,
        &QPushButton::clicked,
        this,
        &ScanPage::onPlusClicked
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
    ui.quantityInput->setValue(1);

    if (mode == ScanMode::ScanIn) {
        ui.titleLabel->setText("Scan Item");
        ui.pageTitleLabel->setText("Scan In");
        ui.breadcrumbLabel->setText("Dashboard  >  Scan In");
        ui.submitButton->setText("⟳  Add Stock");
    }
    else {
        ui.titleLabel->setText("Scan Item");
        ui.pageTitleLabel->setText("Scan Out");
        ui.breadcrumbLabel->setText("Dashboard  >  Scan Out");
        ui.submitButton->setText("⟳  Remove Stock");
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

    ui.statusLabel->setText("Submitting…");

    ProductService* svc = &productService;
    ScanMode submitMode = mode;
    std::string barcodeStd = barcode.toStdString();

    // Perform the scan and the follow-up name lookup off the GUI thread, then
    // update the UI on the GUI thread.
    AsyncTask::run(this,
        [svc, submitMode, barcodeStd, quantity]() {
            bool ok = (submitMode == ScanMode::ScanIn)
                ? svc->scanIn(barcodeStd, quantity)
                : svc->scanOut(barcodeStd, quantity);

            std::string name = ok ? svc->getProductNameByBarcode(barcodeStd) : std::string();
            return std::make_pair(ok, name);
        },
        [this, barcode, quantity](std::pair<bool, std::string> result) {
            if (!result.first) {
                ui.statusLabel->setText("Operation failed.");
                return;
            }

            QString productName = QString::fromStdString(result.second);
            if (productName.trimmed().isEmpty()) {
                productName = "Unknown Product";
            }

            ui.statusLabel->setText("Stock updated successfully.");

            addRecentScan(
                barcode,
                productName,
                quantity
            );

            ui.barcodeInput->clear();
            ui.quantityInput->setValue(1);

            emit stockChanged();
        });
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

    bool hasFirstScan =
        ui.recentCodeLabel->text() != "-" &&
        ui.recentNameLabel->text() != "No scan yet";

    bool hasSecondScan =
        ui.recentCodeLabel_2->text() != "-";

    if (hasSecondScan) {
        ui.recentCodeLabel_3->setText(ui.recentCodeLabel_2->text());
        ui.recentNameLabel_3->setText(ui.recentNameLabel_2->text());
        ui.recentQtyLabel_3->setText(ui.recentQtyLabel_2->text());
        ui.recentDateLabel_3->setText(ui.recentDateLabel_2->text());
    }

    if (hasFirstScan) {
        ui.recentCodeLabel_2->setText(ui.recentCodeLabel->text());
        ui.recentNameLabel_2->setText(ui.recentNameLabel->text());
        ui.recentQtyLabel_2->setText(ui.recentQtyLabel->text());
        ui.recentDateLabel_2->setText(ui.recentDateLabel->text());
    }

    ui.recentCodeLabel->setText(barcode);
    ui.recentNameLabel->setText(productName);
    ui.recentQtyLabel->setText(sign + QString::number(quantity));
    ui.recentDateLabel->setText(date);
}

void ScanPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::scanPageStyle(theme)
    );
}