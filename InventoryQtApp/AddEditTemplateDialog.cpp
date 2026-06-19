#include "AddEditTemplateDialog.h"
#include "IconUtil.h"
#include "AddEditTemplateItemDialog.h"

#include <QAbstractItemView>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QWidget>

AddEditTemplateDialog::AddEditTemplateDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);

    resize(920, 620);
    setMinimumSize(920, 620);
    setMaximumSize(920, 620);

    applyTheme(Theme::AppTheme::Dark);

    ui.errorLabel->setText("");

    setupTable();
    setupConnections();

    ui.templateItemsTable->setRowCount(0);
}

AddEditTemplateDialog::~AddEditTemplateDialog()
{
}

void AddEditTemplateDialog::setupConnections()
{
    connect(
        ui.addItemButton,
        &QPushButton::clicked,
        this,
        &AddEditTemplateDialog::onAddItemClicked
    );

    connect(
        ui.saveButton,
        &QPushButton::clicked,
        this,
        &AddEditTemplateDialog::onSaveClicked
    );

    connect(
        ui.cancelButton,
        &QPushButton::clicked,
        this,
        &AddEditTemplateDialog::onCancelClicked
    );

    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &AddEditTemplateDialog::onCloseClicked
    );
}

void AddEditTemplateDialog::setupTable()
{
    ui.templateItemsTable->setColumnCount(5);

    ui.templateItemsTable->setHorizontalHeaderLabels(
        QStringList()
        << "Item"
        << "Required Qty"
        << "Expected Price"
        << "Minimum Qty"
        << "Actions"
    );

    ui.templateItemsTable->verticalHeader()->setVisible(false);
    ui.templateItemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.templateItemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.templateItemsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.templateItemsTable->setShowGrid(false);
    ui.templateItemsTable->setFrameShape(QFrame::NoFrame);
    ui.templateItemsTable->setFocusPolicy(Qt::NoFocus);
    ui.templateItemsTable->setAlternatingRowColors(false);
    ui.templateItemsTable->viewport()->setAutoFillBackground(false);

    ui.templateItemsTable->horizontalHeader()->setHighlightSections(false);
    ui.templateItemsTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    ui.templateItemsTable->horizontalHeader()->setFixedHeight(42);

    ui.templateItemsTable->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Interactive
    );

    ui.templateItemsTable->setColumnWidth(0, 230);
    ui.templateItemsTable->setColumnWidth(1, 110);
    ui.templateItemsTable->setColumnWidth(2, 125);
    ui.templateItemsTable->setColumnWidth(3, 115);
    ui.templateItemsTable->setColumnWidth(4, 90);

    ui.templateItemsTable->verticalHeader()->setDefaultSectionSize(48);

    ui.templateItemsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.templateItemsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void AddEditTemplateDialog::setEditMode(const QString& templateId)
{
    Q_UNUSED(templateId);

    readOnlyMode = false;

    ui.titleLabel->setText("Edit Template");
    setWindowTitle("Edit Template");

    ui.saveButton->setText("Save Template");
}

QString AddEditTemplateDialog::getTemplateName() const
{
    return ui.templateNameInput->text().trimmed();
}

QString AddEditTemplateDialog::getDescription() const
{
    return ui.descriptionInput->toPlainText().trimmed();
}

QString AddEditTemplateDialog::getTradeType() const
{
    return ui.tradeTypeInput->text().trimmed();
}

std::vector<CreateTemplateItemRequest> AddEditTemplateDialog::getItems() const
{
    std::vector<CreateTemplateItemRequest> items;

    for (int row = 0; row < ui.templateItemsTable->rowCount(); row++) {
        QTableWidgetItem* nameItem =
            ui.templateItemsTable->item(row, 0);

        if (!nameItem || nameItem->text().trimmed().isEmpty()) {
            continue;
        }

        CreateTemplateItemRequest item;

        item.productName =
            nameItem->text().trimmed().toStdString();

        item.category = "";

        item.requiredQuantity =
            ui.templateItemsTable->item(row, 1)
            ? ui.templateItemsTable->item(row, 1)->text().toInt()
            : 1;

        QString priceText =
            ui.templateItemsTable->item(row, 2)
            ? ui.templateItemsTable->item(row, 2)->text()
            : "0";

        priceText.replace("$", "");
        priceText = priceText.trimmed();

        item.expectedPrice =
            priceText.toDouble();

        item.minimumQuantity =
            ui.templateItemsTable->item(row, 3)
            ? ui.templateItemsTable->item(row, 3)->text().toInt()
            : 1;

        item.unit = "";

        item.notes =
            getDescription().toStdString();

        items.push_back(item);
    }

    return items;
}

void AddEditTemplateDialog::onAddItemClicked()
{
    if (readOnlyMode) {
        return;
    }

    AddEditTemplateItemDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        addItemRow(dialog.getItem());
    }
}

void AddEditTemplateDialog::onSaveClicked()
{
    ui.errorLabel->setText("");

    if (readOnlyMode) {
        reject();
        return;
    }

    if (getTemplateName().isEmpty()) {
        ui.errorLabel->setText("Template name is required.");
        return;
    }

    if (getItems().empty()) {
        ui.errorLabel->setText("Add at least one item.");
        return;
    }

    accept();
}

void AddEditTemplateDialog::onCancelClicked()
{
    reject();
}

void AddEditTemplateDialog::onCloseClicked()
{
    reject();
}

void AddEditTemplateDialog::addItemRow(
    const CreateTemplateItemRequest& item
)
{
    int row =
        ui.templateItemsTable->rowCount();

    ui.templateItemsTable->insertRow(row);

    ui.templateItemsTable->setItem(
        row,
        0,
        new QTableWidgetItem(QString::fromStdString(item.productName))
    );

    ui.templateItemsTable->setItem(
        row,
        1,
        new QTableWidgetItem(QString::number(item.requiredQuantity))
    );

    ui.templateItemsTable->setItem(
        row,
        2,
        new QTableWidgetItem("$" + QString::number(item.expectedPrice, 'f', 2))
    );

    ui.templateItemsTable->setItem(
        row,
        3,
        new QTableWidgetItem(QString::number(item.minimumQuantity))
    );

    addItemActionButtons(row);
}

CreateTemplateItemRequest AddEditTemplateDialog::getItemFromRow(
    int row
) const
{
    CreateTemplateItemRequest item;

    if (row < 0 || row >= ui.templateItemsTable->rowCount()) {
        return item;
    }

    item.productName =
        ui.templateItemsTable->item(row, 0)
        ? ui.templateItemsTable->item(row, 0)->text().toStdString()
        : "";

    item.requiredQuantity =
        ui.templateItemsTable->item(row, 1)
        ? ui.templateItemsTable->item(row, 1)->text().toInt()
        : 1;

    QString priceText =
        ui.templateItemsTable->item(row, 2)
        ? ui.templateItemsTable->item(row, 2)->text()
        : "0";

    priceText.replace("$", "");
    priceText = priceText.trimmed();

    item.expectedPrice =
        priceText.toDouble();

    item.minimumQuantity =
        ui.templateItemsTable->item(row, 3)
        ? ui.templateItemsTable->item(row, 3)->text().toInt()
        : 1;

    item.category = "";
    item.unit = "";
    item.notes = "";

    return item;
}

void AddEditTemplateDialog::addItemActionButtons(
    int row
)
{
    QWidget* actionWidget =
        new QWidget(ui.templateItemsTable);

    actionWidget->setObjectName("actionContainer");

    QHBoxLayout* layout =
        new QHBoxLayout(actionWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    layout->setAlignment(Qt::AlignCenter);

    QPushButton* editButton =
        new QPushButton("✎", actionWidget);

    editButton->setObjectName("editButton");
    IconUtil::iconifyActionButton(editButton);
    editButton->setFixedSize(28, 28);
    editButton->setToolTip("Edit");

    QPushButton* deleteButton =
        new QPushButton("🗑", actionWidget);

    deleteButton->setObjectName("deleteButton");
    IconUtil::iconifyActionButton(deleteButton);
    deleteButton->setFixedSize(28, 28);
    deleteButton->setToolTip("Delete");

    layout->addWidget(editButton);
    layout->addWidget(deleteButton);

    ui.templateItemsTable->setCellWidget(
        row,
        4,
        actionWidget
    );

    connect(
        editButton,
        &QPushButton::clicked,
        this,
        [this, row]() {
            onEditItemClicked(row);
        }
    );

    connect(
        deleteButton,
        &QPushButton::clicked,
        this,
        [this, row]() {
            onDeleteItemClicked(row);
        }
    );
}

void AddEditTemplateDialog::onEditItemClicked(
    int row
)
{
    if (readOnlyMode) {
        return;
    }

    AddEditTemplateItemDialog dialog(this);

    dialog.setItem(
        getItemFromRow(row)
    );

    if (dialog.exec() == QDialog::Accepted) {
        CreateTemplateItemRequest item =
            dialog.getItem();

        ui.templateItemsTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::fromStdString(item.productName))
        );

        ui.templateItemsTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::number(item.requiredQuantity))
        );

        ui.templateItemsTable->setItem(
            row,
            2,
            new QTableWidgetItem("$" + QString::number(item.expectedPrice, 'f', 2))
        );

        ui.templateItemsTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::number(item.minimumQuantity))
        );

        addItemActionButtons(row);
    }
}

void AddEditTemplateDialog::onDeleteItemClicked(
    int row
)
{
    if (readOnlyMode) {
        return;
    }

    if (row >= 0 && row < ui.templateItemsTable->rowCount()) {
        ui.templateItemsTable->removeRow(row);
    }
}

void AddEditTemplateDialog::setTemplateData(
    const TemplateDetailsDto& templateDetails,
    bool readOnly
)
{
    readOnlyMode = readOnly;

    ui.titleLabel->setText(
        readOnly ? "Template Details" : "Edit Template"
    );

    setWindowTitle(
        readOnly ? "Template Details" : "Edit Template"
    );

    ui.templateNameInput->setText(
        QString::fromStdString(templateDetails.name)
    );

    ui.tradeTypeInput->setText(
        QString::fromStdString(templateDetails.tradeType)
    );

    ui.templateItemsTable->setRowCount(0);

    for (const auto& item : templateDetails.items) {
        addItemRow(item);
    }

    ui.templateNameInput->setReadOnly(readOnly);
    ui.tradeTypeInput->setReadOnly(readOnly);
    ui.descriptionInput->setReadOnly(readOnly);

    ui.addItemButton->setVisible(!readOnly);
    ui.saveButton->setVisible(!readOnly);

    ui.cancelButton->setText(
        readOnly ? "Close" : "Cancel"
    );
}

void AddEditTemplateDialog::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

void AddEditTemplateDialog::mousePressEvent(
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

void AddEditTemplateDialog::mouseMoveEvent(
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