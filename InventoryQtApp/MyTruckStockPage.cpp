#include "MyTruckStockPage.h"
#include "Theme.h"
#include "UseTruckItemDialog.h"

#include <QAbstractItemView>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>

MyTruckStockPage::MyTruckStockPage(
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);

    applyTheme(Theme::AppTheme::Dark);

    setupTable();
    setupConnections();
    loadStock();
}

MyTruckStockPage::~MyTruckStockPage()
{
}

void MyTruckStockPage::setupConnections()
{
    connect(
        ui.searchInput,
        &QLineEdit::textChanged,
        this,
        &MyTruckStockPage::onSearchChanged
    );
}

void MyTruckStockPage::setupTable()
{
    ui.truckStockTable->setColumnCount(6);

    ui.truckStockTable->setHorizontalHeaderLabels(
        QStringList()
        << "Item"
        << "Category"
        << "Current Qty"
        << "Minimum Qty"
        << "Status"
        << "Actions"
    );

    ui.truckStockTable->verticalHeader()->setVisible(false);
    ui.truckStockTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.truckStockTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.truckStockTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.truckStockTable->setShowGrid(false);
    ui.truckStockTable->setFrameShape(QFrame::NoFrame);
    ui.truckStockTable->setFocusPolicy(Qt::NoFocus);
    ui.truckStockTable->setAlternatingRowColors(false);
    ui.truckStockTable->viewport()->setAutoFillBackground(false);

    ui.truckStockTable->horizontalHeader()->setHighlightSections(false);
    ui.truckStockTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    ui.truckStockTable->horizontalHeader()->setFixedHeight(48);
    ui.truckStockTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.truckStockTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);

    ui.truckStockTable->setColumnWidth(5, 130);
    ui.truckStockTable->verticalHeader()->setDefaultSectionSize(52);

    ui.truckStockTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.truckStockTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void MyTruckStockPage::loadStock()
{
    ui.truckStockTable->clearContents();

    if (!truckStockService) {
        ui.truckStockTable->setRowCount(0);
        return;
    }

    currentStock =
        truckStockService->getMyTruckStock();

    ui.truckStockTable->setRowCount(
        static_cast<int>(currentStock.items.size())
    );

    for (int row = 0; row < static_cast<int>(currentStock.items.size()); row++) {
        const MyTruckStockItemDto& item =
            currentStock.items[row];

        ui.truckStockTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::fromStdString(item.productName))
        );

        ui.truckStockTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(item.category))
        );

        ui.truckStockTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::number(item.currentQuantity))
        );

        ui.truckStockTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::number(item.minimumQuantity))
        );

        ui.truckStockTable->setItem(
            row,
            4,
            new QTableWidgetItem(QString::fromStdString(item.status))
        );

        addUseButton(row);
    }
}

void MyTruckStockPage::refreshStock()
{
    loadStock();
}

void MyTruckStockPage::onSearchChanged(
    const QString& text
)
{
    QString searchText =
        text.trimmed();

    for (int row = 0; row < ui.truckStockTable->rowCount(); row++) {
        bool match = searchText.isEmpty();

        for (int col = 0; col < ui.truckStockTable->columnCount(); col++) {
            QTableWidgetItem* item =
                ui.truckStockTable->item(row, col);

            if (item && item->text().contains(searchText, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }

        ui.truckStockTable->setRowHidden(row, !match);
    }
}

void MyTruckStockPage::addUseButton(
    int row
)
{
    QWidget* actionWidget =
        new QWidget(this);

    actionWidget->setObjectName("actionContainer");

    QHBoxLayout* layout =
        new QHBoxLayout(actionWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->setAlignment(Qt::AlignCenter);

    QPushButton* useButton =
        new QPushButton("Use", actionWidget);

    useButton->setObjectName("useButton");

    layout->addWidget(useButton);

    ui.truckStockTable->setCellWidget(row, 5, actionWidget);

    connect(useButton, &QPushButton::clicked, this, [this, row]() {
        onUseItemClicked(row);
        });
}

void MyTruckStockPage::onUseItemClicked(
    int row
)
{
    if (row < 0 || row >= static_cast<int>(currentStock.items.size())) {
        return;
    }

    const MyTruckStockItemDto& item =
        currentStock.items[row];

    UseTruckItemDialog dialog(this);

    dialog.setItem(
        QString::fromStdString(item.productName),
        item.currentQuantity
    );

    if (dialog.exec() == QDialog::Accepted) {
        UseTruckItemRequest request;

        request.itemId =
            item.id;

        request.quantityUsed =
            dialog.getQuantityToUse();

        request.notes =
            dialog.getNotes().toStdString();

        bool success =
            truckStockService->useTruckItem(request);

        if (success) {
            QMessageBox::information(
                this,
                "Success",
                "Item usage recorded successfully."
            );

            loadStock();
        }
        else {
            QMessageBox::warning(
                this,
                "Error",
                "Failed to use item. Check backend console."
            );
        }
    }
}

void MyTruckStockPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::myTruckStockPageStyle(theme)
    );
}