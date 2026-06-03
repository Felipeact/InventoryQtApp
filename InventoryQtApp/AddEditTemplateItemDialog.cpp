#include "AddEditTemplateItemDialog.h"

#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>

AddEditTemplateItemDialog::AddEditTemplateItemDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);

    resize(620, 520);
    setMinimumSize(620, 520);
    setMaximumSize(620, 520);

    applyTheme(Theme::AppTheme::Dark);

    ui.errorLabel->setText("");

    setupConnections();
}

AddEditTemplateItemDialog::~AddEditTemplateItemDialog()
{
}

void AddEditTemplateItemDialog::setupConnections()
{
    connect(
        ui.saveButton,
        &QPushButton::clicked,
        this,
        &AddEditTemplateItemDialog::onSaveClicked
    );

    connect(
        ui.cancelButton,
        &QPushButton::clicked,
        this,
        &AddEditTemplateItemDialog::onCancelClicked
    );

    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &AddEditTemplateItemDialog::onCloseClicked
    );
}

CreateTemplateItemRequest AddEditTemplateItemDialog::getItem() const
{
    CreateTemplateItemRequest item;

    item.productName =
        ui.itemNameInput->text().trimmed().toStdString();

    item.category =
        ui.categoryInput->text().trimmed().toStdString();

    item.requiredQuantity =
        ui.requiredQtyInput->value();

    item.minimumQuantity =
        ui.minimumQtyInput->value();

    item.expectedPrice =
        ui.expectedPriceInput->value();

    item.unit =
        ui.unitInput->text().trimmed().toStdString();

    item.notes =
        ui.notesInput->toPlainText().trimmed().toStdString();

    return item;
}

void AddEditTemplateItemDialog::setItem(
    const CreateTemplateItemRequest& item
)
{
    ui.titleLabel->setText("Edit Template Item");
    setWindowTitle("Edit Template Item");

    ui.itemNameInput->setText(
        QString::fromStdString(item.productName)
    );

    ui.categoryInput->setText(
        QString::fromStdString(item.category)
    );

    ui.requiredQtyInput->setValue(
        item.requiredQuantity
    );

    ui.minimumQtyInput->setValue(
        item.minimumQuantity
    );

    ui.expectedPriceInput->setValue(
        item.expectedPrice
    );

    ui.unitInput->setText(
        QString::fromStdString(item.unit)
    );

    ui.notesInput->setPlainText(
        QString::fromStdString(item.notes)
    );
}

bool AddEditTemplateItemDialog::validateForm()
{
    ui.errorLabel->setText("");

    if (ui.itemNameInput->text().trimmed().isEmpty()) {
        ui.errorLabel->setText("Item name is required.");
        return false;
    }

    return true;
}

void AddEditTemplateItemDialog::onSaveClicked()
{
    if (!validateForm()) {
        return;
    }

    accept();
}

void AddEditTemplateItemDialog::onCancelClicked()
{
    reject();
}

void AddEditTemplateItemDialog::onCloseClicked()
{
    reject();
}

void AddEditTemplateItemDialog::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

void AddEditTemplateItemDialog::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition =
            event->globalPosition().toPoint()
            - frameGeometry().topLeft();

        event->accept();
    }
}

void AddEditTemplateItemDialog::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(
            event->globalPosition().toPoint()
            - dragPosition
        );

        event->accept();
    }
}