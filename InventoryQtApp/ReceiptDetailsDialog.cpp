#include "ReceiptDetailsDialog.h"
#include "Theme.h"

#include <QDesktopServices>
#include <QFileInfo>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QUrl>

ReceiptDetailsDialog::ReceiptDetailsDialog(
    const ReceiptDto& receipt,
    QWidget* parent
)
    : QDialog(parent),
    receipt(receipt)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);

    resize(820, 680);
    setMinimumSize(820, 680);
    setMaximumSize(820, 680);

    applyTheme(Theme::AppTheme::Dark);

    setupConnections();
    loadReceiptDetails();
    loadPreview();
}

ReceiptDetailsDialog::~ReceiptDetailsDialog()
{
}

void ReceiptDetailsDialog::setupConnections()
{
    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &ReceiptDetailsDialog::onCloseClicked
    );

    connect(
        ui.closeBottomButton,
        &QPushButton::clicked,
        this,
        &ReceiptDetailsDialog::onCloseClicked
    );

    connect(
        ui.openFileButton,
        &QPushButton::clicked,
        this,
        &ReceiptDetailsDialog::onOpenFileClicked
    );
}

void ReceiptDetailsDialog::loadReceiptDetails()
{
    ui.receiptIdValue->setText(
        QString::fromStdString(receipt.id)
    );

    ui.technicianValue->setText(
        QString::fromStdString(receipt.technicianName)
    );

    ui.truckValue->setText(
        QString::fromStdString(receipt.truckNumber)
    );

    ui.amountValue->setText(
        "$" + QString::number(receipt.totalAmount, 'f', 2)
    );

    ui.statusValue->setText(
        QString::fromStdString(receipt.status)
    );

    ui.dateValue->setText(
        QString::fromStdString(receipt.createdAt)
    );

    QString fileUrl =
        QString::fromStdString(receipt.fileUrl);

    if (fileUrl.trimmed().isEmpty()) {
        ui.fileValue->setText("No file attached");
        ui.openFileButton->setEnabled(false);
    }
    else {
        ui.fileValue->setText(fileUrl);
        ui.openFileButton->setEnabled(true);
    }
}

void ReceiptDetailsDialog::loadPreview()
{
    QString fileUrl =
        QString::fromStdString(receipt.fileUrl).trimmed();

    if (fileUrl.isEmpty()) {
        ui.previewLabel->setText("No receipt file attached.");
        return;
    }

    if (
        fileUrl.startsWith("http://", Qt::CaseInsensitive) ||
        fileUrl.startsWith("https://", Qt::CaseInsensitive)
        ) {
        ui.previewLabel->setText(
            "Online receipt file.\nClick Open File to view it."
        );
        return;
    }

    QFileInfo fileInfo(fileUrl);

    if (!fileInfo.exists()) {
        ui.previewLabel->setText(
            "Receipt file path saved, but the file was not found locally.\nClick Open File to try opening it."
        );
        return;
    }

    if (isImageFile(fileUrl)) {
        QPixmap pixmap(fileUrl);

        if (pixmap.isNull()) {
            ui.previewLabel->setText("Unable to load image preview.");
            return;
        }

        ui.previewLabel->setPixmap(
            pixmap.scaled(
                ui.previewLabel->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            )
        );

        return;
    }

    if (isPdfFile(fileUrl)) {
        ui.previewLabel->setText(
            "PDF receipt detected.\nClick Open File to view it."
        );
        return;
    }

    ui.previewLabel->setText(
        "Preview not available for this file type.\nClick Open File to view it."
    );
}

bool ReceiptDetailsDialog::isImageFile(
    const QString& filePath
) const
{
    QString lower =
        filePath.toLower();

    return lower.endsWith(".png") ||
        lower.endsWith(".jpg") ||
        lower.endsWith(".jpeg") ||
        lower.endsWith(".bmp") ||
        lower.endsWith(".webp");
}

bool ReceiptDetailsDialog::isPdfFile(
    const QString& filePath
) const
{
    return filePath.toLower().endsWith(".pdf");
}

void ReceiptDetailsDialog::onOpenFileClicked()
{
    QString fileUrl =
        QString::fromStdString(receipt.fileUrl).trimmed();

    if (fileUrl.isEmpty()) {
        return;
    }

    if (
        fileUrl.startsWith("http://", Qt::CaseInsensitive) ||
        fileUrl.startsWith("https://", Qt::CaseInsensitive)
        ) {
        QDesktopServices::openUrl(
            QUrl(fileUrl)
        );

        return;
    }

    QDesktopServices::openUrl(
        QUrl::fromLocalFile(fileUrl)
    );
}

void ReceiptDetailsDialog::onCloseClicked()
{
    reject();
}

void ReceiptDetailsDialog::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

void ReceiptDetailsDialog::mousePressEvent(
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

void ReceiptDetailsDialog::mouseMoveEvent(
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