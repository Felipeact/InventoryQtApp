#include "ItemDetailsDialog.h"

#include <QFileInfo>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPixmap>
#include <QPushButton>
#include <QUrl>

ItemDetailsDialog::ItemDetailsDialog(
    const json& product,
    QWidget* parent
)
    : QDialog(parent),
    product(product)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);

    resize(760, 560);
    setMinimumSize(760, 560);
    setMaximumSize(760, 560);

    applyTheme(Theme::AppTheme::Dark);

    setupConnections();
    loadProductDetails();
    loadImagePreview();
}

ItemDetailsDialog::~ItemDetailsDialog()
{
}

void ItemDetailsDialog::setupConnections()
{
    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &ItemDetailsDialog::onCloseClicked
    );

    connect(
        ui.closeBottomButton,
        &QPushButton::clicked,
        this,
        &ItemDetailsDialog::onCloseClicked
    );
}

void ItemDetailsDialog::loadProductDetails()
{
    ui.nameValue->setText(
        getStringValue("name")
    );

    ui.barcodeValue->setText(
        getStringValue("barcode")
    );

    ui.quantityValue->setText(
        QString::number(getQuantity()) + " pcs"
    );

    ui.locationValue->setText(
        getStringValue("location", "Main Store")
    );

    ui.typeValue->setText(
        getStringValue("type", "Product")
    );

    ui.projectValue->setText(
        getStringValue("project", "HQ")
    );

    ui.statusValue->setText(
        getStringValue("status", "Activated")
    );

    ui.imagePathValue->setText(
        getImageValue().isEmpty()
        ? "No image"
        : getImageValue()
    );
}

void ItemDetailsDialog::loadImagePreview()
{
    QString imageValue =
        getImageValue().trimmed();

    if (imageValue.isEmpty()) {
        ui.imagePreviewLabel->setText("No image available.");
        return;
    }

    if (
        imageValue.startsWith("http://", Qt::CaseInsensitive) ||
        imageValue.startsWith("https://", Qt::CaseInsensitive)
        ) {
        QNetworkAccessManager* manager =
            new QNetworkAccessManager(ui.imagePreviewLabel);

        QUrl imageUrl(imageValue);

        QNetworkRequest request;
        request.setUrl(imageUrl);

        QNetworkReply* reply =
            manager->get(request);

        connect(
            reply,
            &QNetworkReply::finished,
            ui.imagePreviewLabel,
            [reply, this]() {
                QByteArray imageData =
                    reply->readAll();

                QPixmap pixmap;

                if (pixmap.loadFromData(imageData)) {
                    ui.imagePreviewLabel->setText("");

                    ui.imagePreviewLabel->setPixmap(
                        pixmap.scaled(
                            ui.imagePreviewLabel->size(),
                            Qt::KeepAspectRatio,
                            Qt::SmoothTransformation
                        )
                    );
                }
                else {
                    ui.imagePreviewLabel->setText("Unable to load image.");
                }

                reply->deleteLater();
            }
        );

        return;
    }

    QFileInfo fileInfo(imageValue);

    if (!fileInfo.exists()) {
        ui.imagePreviewLabel->setText("Image file not found.");
        return;
    }

    QPixmap pixmap(imageValue);

    if (pixmap.isNull()) {
        ui.imagePreviewLabel->setText("Unable to load image.");
        return;
    }

    ui.imagePreviewLabel->setText("");

    ui.imagePreviewLabel->setPixmap(
        pixmap.scaled(
            ui.imagePreviewLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        )
    );
}

QString ItemDetailsDialog::getStringValue(
    const std::string& key,
    const QString& fallback
) const
{
    if (
        product.contains(key) &&
        product[key].is_string()
        ) {
        std::string value =
            product.value(key, "");

        if (!value.empty()) {
            return QString::fromStdString(value);
        }
    }

    return fallback;
}

int ItemDetailsDialog::getQuantity() const
{
    if (
        product.contains("inventory") &&
        !product["inventory"].is_null()
        ) {
        return product["inventory"].value("quantity", 0);
    }

    return product.value("quantity", 0);
}

QString ItemDetailsDialog::getImageValue() const
{
    if (product.contains("imageUrl")) {
        return QString::fromStdString(
            product.value("imageUrl", "")
        );
    }

    if (product.contains("image")) {
        return QString::fromStdString(
            product.value("image", "")
        );
    }

    if (product.contains("photoUrl")) {
        return QString::fromStdString(
            product.value("photoUrl", "")
        );
    }

    if (product.contains("thumbnailUrl")) {
        return QString::fromStdString(
            product.value("thumbnailUrl", "")
        );
    }

    return "";
}

void ItemDetailsDialog::onCloseClicked()
{
    reject();
}

void ItemDetailsDialog::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

void ItemDetailsDialog::mousePressEvent(
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

void ItemDetailsDialog::mouseMoveEvent(
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