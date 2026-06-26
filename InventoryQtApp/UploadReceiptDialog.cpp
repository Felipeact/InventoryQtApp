#include "UploadReceiptDialog.h"

#include <QApplication>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include <QPushButton>

UploadReceiptDialog::UploadReceiptDialog(
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QDialog(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);

    resize(820, 680);
    setMinimumSize(820, 680);
    setMaximumSize(820, 680);

    applyTheme(Theme::AppTheme::Dark);

    selectedFilePath = "";

    ui.errorLabel->setText("");
    ui.summaryValue->setText("0");
    ui.summaryValue2->setText("$0.00");

    setupConnections();
    loadTrucks();
}

UploadReceiptDialog::~UploadReceiptDialog()
{
}

void UploadReceiptDialog::setupConnections()
{
    connect(
        ui.browseButton,
        &QPushButton::clicked,
        this,
        &UploadReceiptDialog::onBrowseClicked
    );

    connect(
        ui.uploadButton,
        &QPushButton::clicked,
        this,
        &UploadReceiptDialog::onUploadClicked
    );

    connect(
        ui.cancelButton,
        &QPushButton::clicked,
        this,
        &UploadReceiptDialog::onCancelClicked
    );

    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &UploadReceiptDialog::onCloseClicked
    );

    connect(
        ui.truckComboBox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &UploadReceiptDialog::onTruckSelected
    );
}

void UploadReceiptDialog::loadTrucks()
{
    ui.truckComboBox->clear();

    ui.truckComboBox->addItem(
        "Select Truck",
        ""
    );

    if (!truckStockService) {
        return;
    }

    std::vector<TruckDto> trucks =
        truckStockService->getTrucks();

    for (const TruckDto& truck : trucks) {
        ui.truckComboBox->addItem(
            QString::fromStdString(truck.truckName),
            QString::fromStdString(truck.id)
        );
    }
}

void UploadReceiptDialog::onBrowseClicked()
{
    QString fileName =
        QFileDialog::getOpenFileName(
            this,
            "Select Receipt File",
            "",
            "Receipt Files (*.png *.jpg *.jpeg *.bmp *.pdf *.xlsx *.xls);;All Files (*)"
        );

    if (!fileName.isEmpty()) {
        updateFileDisplay(fileName);
        loadPreview(fileName);
        runExtraction(fileName);
    }
}

void UploadReceiptDialog::runExtraction(
    const QString& filePath
)
{
    extractedItems.clear();
    ui.summaryValue->setText("0");
    ui.summaryValue2->setText("$0.00");

    if (!truckStockService) {
        return;
    }

    ui.errorLabel->setText("Reading receipt with AI...");
    ui.browseButton->setEnabled(false);
    QApplication::processEvents();

    ExtractedReceiptDto result =
        truckStockService->extractReceipt(filePath.toStdString());

    ui.browseButton->setEnabled(true);

    if (!result.ok) {
        ui.errorLabel->setText(
            "Could not auto-read this receipt. Enter the amount manually."
        );
        return;
    }

    extractedItems = result.items;

    if (result.hasTotal && result.total > 0) {
        ui.amountInput->setValue(result.total);
        ui.summaryValue2->setText(
            "$" + QString::number(result.total, 'f', 2)
        );
    }

    ui.summaryValue->setText(
        QString::number(static_cast<int>(result.items.size()))
    );

    QString message =
        QString("AI read %1 item(s)")
            .arg(static_cast<int>(result.items.size()));

    if (!result.supplier.empty()) {
        message += " from " + QString::fromStdString(result.supplier);
    }

    message += ". Review the amount before uploading.";

    ui.errorLabel->setText(message);
}

void UploadReceiptDialog::updateFileDisplay(
    const QString& filePath
)
{
    selectedFilePath = filePath;

    QFileInfo fileInfo(filePath);

    ui.uploadText->setText(
        fileInfo.fileName()
    );

    ui.uploadSubtext->setText(
        filePath
    );
}

void UploadReceiptDialog::loadPreview(
    const QString& filePath
)
{
    Q_UNUSED(filePath);

    // Preview can be added later.
}

QString UploadReceiptDialog::getSelectedTruck() const
{
    return ui.truckComboBox
        ->currentText();
}

QString UploadReceiptDialog::getSelectedTruckId() const
{
    return ui.truckComboBox
        ->currentData()
        .toString();
}

QString UploadReceiptDialog::getTotalAmount() const
{
    return QString::number(
        ui.amountInput->value(),
        'f',
        2
    );
}

QString UploadReceiptDialog::getFilePath() const
{
    return selectedFilePath;
}

std::vector<ExtractedReceiptItemDto>
UploadReceiptDialog::getExtractedItems() const
{
    return extractedItems;
}

void UploadReceiptDialog::onUploadClicked()
{
    ui.errorLabel->setText("");

    if (ui.truckComboBox->currentIndex() <= 0) {
        ui.errorLabel->setText("Please select a truck.");
        return;
    }

    if (selectedFilePath.isEmpty()) {
        ui.errorLabel->setText("Please select a receipt file.");
        return;
    }

    if (ui.amountInput->value() <= 0) {
        ui.errorLabel->setText("Please enter the total amount.");
        return;
    }

    accept();
}

void UploadReceiptDialog::onCancelClicked()
{
    reject();
}

void UploadReceiptDialog::onCloseClicked()
{
    reject();
}

void UploadReceiptDialog::onTruckSelected(
    int index
)
{
    Q_UNUSED(index);
}

void UploadReceiptDialog::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

void UploadReceiptDialog::mousePressEvent(
    QMouseEvent* event
)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition =
            event->globalPosition().toPoint()
            - frameGeometry().topLeft();

        event->accept();
    }
}

void UploadReceiptDialog::mouseMoveEvent(
    QMouseEvent* event
)
{
    if (event->buttons() & Qt::LeftButton) {
        move(
            event->globalPosition().toPoint()
            - dragPosition
        );

        event->accept();
    }
}