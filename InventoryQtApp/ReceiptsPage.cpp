#include "ReceiptsPage.h"
#include "UploadReceiptDialog.h"

#include <QComboBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>

ReceiptsPage::ReceiptsPage(
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);
    setupConnections();
    loadReceipts();
}

ReceiptsPage::~ReceiptsPage()
{
}

void ReceiptsPage::setupConnections()
{
    connect(ui.uploadButton, &QPushButton::clicked,
        this, &ReceiptsPage::onUploadReceiptClicked);

    connect(ui.searchInput, &QLineEdit::textChanged,
        this, &ReceiptsPage::onSearchChanged);

    connect(ui.statusFilter,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &ReceiptsPage::onStatusFilterChanged);
}

void ReceiptsPage::loadReceipts()
{
    ui.receiptsTable->clearContents();

    currentReceipts = truckStockService->getReceipts();

    ui.receiptsTable->setRowCount(
        static_cast<int>(currentReceipts.size())
    );

    ui.receiptsTable->setColumnCount(7);

    for (int row = 0; row < static_cast<int>(currentReceipts.size()); ++row) {
        const ReceiptDto& receipt = currentReceipts[row];

        ui.receiptsTable->setItem(row, 0,
            new QTableWidgetItem(QString::fromStdString(receipt.id)));

        ui.receiptsTable->setItem(row, 1,
            new QTableWidgetItem(QString::fromStdString(receipt.technicianName)));

        ui.receiptsTable->setItem(row, 2,
            new QTableWidgetItem(QString::fromStdString(receipt.truckNumber)));

        ui.receiptsTable->setItem(row, 3,
            new QTableWidgetItem(QString::number(receipt.totalAmount, 'f', 2)));

        ui.receiptsTable->setItem(row, 4,
            new QTableWidgetItem(QString::fromStdString(receipt.status)));

        ui.receiptsTable->setItem(row, 5,
            new QTableWidgetItem(QString::fromStdString(receipt.createdAt)));

        ui.receiptsTable->setItem(row, 6,
            new QTableWidgetItem("View / Approve"));
    }

    ui.receiptsTable->horizontalHeader()->setStretchLastSection(true);
    ui.receiptsTable->verticalHeader()->setVisible(false);
}

void ReceiptsPage::refreshReceipts()
{
    loadReceipts();
}

void ReceiptsPage::filterReceipts()
{
    QString searchText = ui.searchInput->text().trimmed();
    QString selectedStatus = ui.statusFilter->currentText();

    for (int row = 0; row < ui.receiptsTable->rowCount(); ++row) {
        bool searchMatch = searchText.isEmpty();
        bool statusMatch = selectedStatus == "All Status";

        for (int col = 0; col < ui.receiptsTable->columnCount(); ++col) {
            QTableWidgetItem* item = ui.receiptsTable->item(row, col);

            if (item && item->text().contains(searchText, Qt::CaseInsensitive)) {
                searchMatch = true;
            }
        }

        QTableWidgetItem* statusItem = ui.receiptsTable->item(row, 4);

        if (statusItem && statusItem->text() == selectedStatus) {
            statusMatch = true;
        }

        ui.receiptsTable->setRowHidden(row, !(searchMatch && statusMatch));
    }
}

void ReceiptsPage::onUploadReceiptClicked()
{
    UploadReceiptDialog dialog(truckStockService, this);

    if (dialog.exec() == QDialog::Accepted) {

        CreateReceiptRequest request;

        request.truckId =
            dialog.getSelectedTruckId().toStdString();

        request.fileUrl =
            dialog.getFilePath().toStdString();

        request.totalAmount =
            dialog.getTotalAmount().toDouble();

        bool success =
            truckStockService->createReceipt(request);

        if (success) {
            QMessageBox::information(
                this,
                "Success",
                "Receipt uploaded successfully."
            );

            loadReceipts();
        }
        else {
            QMessageBox::warning(
                this,
                "Error",
                "Failed to upload receipt."
            );
        }
    }
}

void ReceiptsPage::onSearchChanged(const QString& text)
{
    Q_UNUSED(text);
    filterReceipts();
}

void ReceiptsPage::onStatusFilterChanged(int index)
{
    Q_UNUSED(index);
    filterReceipts();
}