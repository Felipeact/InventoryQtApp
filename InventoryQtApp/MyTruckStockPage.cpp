#include "MyTruckStockPage.h"
#include "Theme.h"
#include "UseTruckItemDialog.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHBoxLayout>

MyTruckStockPage::MyTruckStockPage(
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);
    setupConnections();

}

MyTruckStockPage::~MyTruckStockPage()
{
}

void MyTruckStockPage::setupConnections()
{
    connect(ui.searchInput, &QLineEdit::textChanged,
        this, &MyTruckStockPage::onSearchChanged);
}

void MyTruckStockPage::loadStock()
{
    ui.truckStockTable->clearContents();

    currentStock = truckStockService->getMyTruckStock();

    ui.truckStockTable->setRowCount(
        static_cast<int>(currentStock.items.size())
    );

    ui.truckStockTable->setColumnCount(6);

    for (int row = 0; row < static_cast<int>(currentStock.items.size()); ++row) {
        const MyTruckStockItemDto& item = currentStock.items[row];

        ui.truckStockTable->setItem(row, 0,
            new QTableWidgetItem(QString::fromStdString(item.productName)));

        ui.truckStockTable->setItem(row, 1,
            new QTableWidgetItem(QString::fromStdString(item.category)));

        ui.truckStockTable->setItem(row, 2,
            new QTableWidgetItem(QString::number(item.currentQuantity)));

        ui.truckStockTable->setItem(row, 3,
            new QTableWidgetItem(QString::number(item.minimumQuantity)));

        ui.truckStockTable->setItem(row, 4,
            new QTableWidgetItem(QString::fromStdString(item.status)));

        addUseButton(row);
    }

    ui.truckStockTable->horizontalHeader()->setStretchLastSection(true);
    ui.truckStockTable->verticalHeader()->setVisible(false);
}

void MyTruckStockPage::refreshStock()
{
    loadStock();
}

void MyTruckStockPage::onSearchChanged(const QString& text)
{
    for (int row = 0; row < ui.truckStockTable->rowCount(); ++row) {
        bool match = false;

        for (int col = 0; col < ui.truckStockTable->columnCount(); ++col) {
            QTableWidgetItem* item = ui.truckStockTable->item(row, col);

            if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }

        ui.truckStockTable->setRowHidden(row, !match);
    }
}

void MyTruckStockPage::addUseButton(int row)
{
    QPushButton* useButton = new QPushButton("Use Item", this);

    ui.truckStockTable->setCellWidget(row, 5, useButton);

    connect(useButton, &QPushButton::clicked, this, [this, row]() {
        onUseItemClicked(row);
        });
}

void MyTruckStockPage::onUseItemClicked(int row)
{
    if (row < 0 || row >= static_cast<int>(currentStock.items.size())) {
        return;
    }

    const MyTruckStockItemDto& item = currentStock.items[row];

    UseTruckItemDialog dialog(this);

    dialog.setItem(
        QString::fromStdString(item.productName),
        item.currentQuantity
    );

    if (dialog.exec() == QDialog::Accepted) {

        UseTruckItemRequest request;

        request.itemId = item.id;
        request.quantityUsed = dialog.getQuantityToUse();
        request.notes = dialog.getNotes().toStdString();

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

void MyTruckStockPage::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::truckPageStyle(theme)
    );
}

