#include "UploadReceiptDialog.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPlainTextEdit>
#include <QPushButton>

UploadReceiptDialog::UploadReceiptDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    setWindowTitle("Upload Receipt");
    setModal(true);
    resize(820, 640);
    setMinimumSize(820, 640);

    selectedFilePath.clear();

    setupConnections();
    loadTrucks();
}

UploadReceiptDialog::~UploadReceiptDialog()
{
}

void UploadReceiptDialog::setupConnections()
{
    connect(ui.browseButton, &QPushButton::clicked,
        this, &UploadReceiptDialog::onBrowseClicked);

    connect(ui.uploadButton, &QPushButton::clicked,
        this, &UploadReceiptDialog::onUploadClicked);

    connect(ui.cancelButton, &QPushButton::clicked,
        this, &UploadReceiptDialog::onCancelClicked);

    connect(ui.truckComboBox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &UploadReceiptDialog::onTruckSelected);
}

void UploadReceiptDialog::loadTrucks()
{
    ui.truckComboBox->clear();
    ui.truckComboBox->addItem("Select Truck");
    ui.truckComboBox->addItem("Truck 1");
    ui.truckComboBox->addItem("Truck 2");
    ui.truckComboBox->addItem("Truck 3");
}

void UploadReceiptDialog::onBrowseClicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select Receipt File",
        "",
        "Receipt Files (*.png *.jpg *.jpeg *.bmp *.pdf *.xlsx *.xls);;All Files (*)"
    );

    if (!fileName.isEmpty()) {
        updateFileDisplay(fileName);
    }
}

void UploadReceiptDialog::updateFileDisplay(const QString& filePath)
{
    selectedFilePath = filePath;

    QFileInfo fileInfo(filePath);

    // Your current redesigned XML does not have fileNameLabel/filePathLabel/previewLabel.
    // So for now we show the selected file in the upload text area.
    ui.uploadText->setText(fileInfo.fileName());
    ui.uploadSubtext->setText(filePath);
}

void UploadReceiptDialog::loadPreview(const QString& filePath)
{
    Q_UNUSED(filePath);

    // Current XML has no previewLabel.
    // Later we can add a real preview area if needed.
}

QString UploadReceiptDialog::getSelectedTruck() const
{
    return ui.truckComboBox->currentText();
}

QString UploadReceiptDialog::getTotalAmount() const
{
    return QString::number(ui.amountInput->value(), 'f', 2);
}

QString UploadReceiptDialog::getFilePath() const
{
    return selectedFilePath;
}

void UploadReceiptDialog::onUploadClicked()
{
    if (ui.truckComboBox->currentIndex() == 0) {
        QMessageBox::warning(this, "Missing Truck", "Please select a truck.");
        return;
    }

    if (selectedFilePath.isEmpty()) {
        QMessageBox::warning(this, "Missing File", "Please select a receipt file.");
        return;
    }

    if (ui.amountInput->value() <= 0) {
        QMessageBox::warning(this, "Missing Amount", "Please enter the total amount.");
        return;
    }

    accept();
}

void UploadReceiptDialog::onCancelClicked()
{
    reject();
}

void UploadReceiptDialog::onTruckSelected(int index)
{
    Q_UNUSED(index);
}