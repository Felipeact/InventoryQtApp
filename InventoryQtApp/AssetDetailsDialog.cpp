#include "AssetDetailsDialog.h"

#include <QMouseEvent>
#include <QPushButton>

AssetDetailsDialog::AssetDetailsDialog(
    const json& asset,
    QWidget* parent
)
    : QDialog(parent),
    asset(asset)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);

    resize(720, 500);
    setMinimumSize(720, 500);
    setMaximumSize(720, 500);

    applyTheme(Theme::AppTheme::Dark);

    setupConnections();
    loadAssetDetails();
}

AssetDetailsDialog::~AssetDetailsDialog()
{
}

void AssetDetailsDialog::setupConnections()
{
    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &AssetDetailsDialog::onCloseClicked
    );

    connect(
        ui.closeBottomButton,
        &QPushButton::clicked,
        this,
        &AssetDetailsDialog::onCloseClicked
    );
}

void AssetDetailsDialog::loadAssetDetails()
{
    ui.nameValue->setText(
        getStringValue("name")
    );

    ui.typeValue->setText(
        getStringValue("type")
    );

    ui.serialCodeValue->setText(
        getStringValue("serialCode")
    );

    ui.statusValue->setText(
        getStringValue("status", "active")
    );

    ui.createdValue->setText(
        getDateOnly(
            getStringValue("createdAt")
        )
    );

    ui.updatedValue->setText(
        getDateOnly(
            getStringValue("updatedAt")
        )
    );

    ui.descriptionValue->setPlainText(
        getStringValue("description", "No description.")
    );
}

QString AssetDetailsDialog::getStringValue(
    const std::string& key,
    const QString& fallback
) const
{
    if (
        asset.contains(key) &&
        asset[key].is_string()
        ) {
        std::string value =
            asset.value(key, "");

        if (!value.empty()) {
            return QString::fromStdString(value);
        }
    }

    return fallback;
}

QString AssetDetailsDialog::getDateOnly(
    const QString& value
) const
{
    if (value.length() >= 10) {
        return value.left(10);
    }

    return value;
}

void AssetDetailsDialog::onCloseClicked()
{
    reject();
}

void AssetDetailsDialog::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

void AssetDetailsDialog::mousePressEvent(
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

void AssetDetailsDialog::mouseMoveEvent(
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