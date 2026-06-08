#include "ItemAdvancedFilterDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QSpinBox>

ItemAdvancedFilterDialog::ItemAdvancedFilterDialog(
    const ItemAdvancedFilters& currentFilters,
    QWidget* parent
)
    : QDialog(parent),
    filters(currentFilters)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);

    resize(640, 430);
    setMinimumSize(640, 430);
    setMaximumSize(640, 430);

    applyTheme(Theme::AppTheme::Dark);

    setupConnections();
    loadFilters();
}

ItemAdvancedFilterDialog::~ItemAdvancedFilterDialog()
{
}

void ItemAdvancedFilterDialog::setupConnections()
{
    connect(
        ui.applyButton,
        &QPushButton::clicked,
        this,
        &ItemAdvancedFilterDialog::onApplyClicked
    );

    connect(
        ui.clearButton,
        &QPushButton::clicked,
        this,
        &ItemAdvancedFilterDialog::onClearClicked
    );

    connect(
        ui.cancelButton,
        &QPushButton::clicked,
        this,
        &ItemAdvancedFilterDialog::onCancelClicked
    );

    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &ItemAdvancedFilterDialog::onCloseClicked
    );
}

void ItemAdvancedFilterDialog::loadFilters()
{
    ui.minQuantityCheckBox->setChecked(
        filters.useMinQuantity
    );

    ui.maxQuantityCheckBox->setChecked(
        filters.useMaxQuantity
    );

    ui.minQuantityInput->setValue(
        filters.minQuantity
    );

    ui.maxQuantityInput->setValue(
        filters.maxQuantity
    );

    int stockIndex =
        ui.stockStatusComboBox->findText(filters.stockStatus);

    if (stockIndex >= 0) {
        ui.stockStatusComboBox->setCurrentIndex(stockIndex);
    }

    ui.storeInput->setText(
        filters.storeText
    );

    ui.typeInput->setText(
        filters.typeText
    );
}

ItemAdvancedFilters ItemAdvancedFilterDialog::getFilters() const
{
    return filters;
}

void ItemAdvancedFilterDialog::onApplyClicked()
{
    filters.enabled = true;

    filters.useMinQuantity =
        ui.minQuantityCheckBox->isChecked();

    filters.useMaxQuantity =
        ui.maxQuantityCheckBox->isChecked();

    filters.minQuantity =
        ui.minQuantityInput->value();

    filters.maxQuantity =
        ui.maxQuantityInput->value();

    filters.stockStatus =
        ui.stockStatusComboBox->currentText();

    filters.storeText =
        ui.storeInput->text().trimmed();

    filters.typeText =
        ui.typeInput->text().trimmed();

    accept();
}

void ItemAdvancedFilterDialog::onClearClicked()
{
    filters = ItemAdvancedFilters();

    loadFilters();

    accept();
}

void ItemAdvancedFilterDialog::onCancelClicked()
{
    reject();
}

void ItemAdvancedFilterDialog::onCloseClicked()
{
    reject();
}

void ItemAdvancedFilterDialog::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

void ItemAdvancedFilterDialog::mousePressEvent(
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

void ItemAdvancedFilterDialog::mouseMoveEvent(
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