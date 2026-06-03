#include "UploadReceiptDialog.h"

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
    }
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