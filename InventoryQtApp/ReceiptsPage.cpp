#include "ReceiptsPage.h"
#include "UploadReceiptDialog.h"

#include <QComboBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHBoxLayout>
#include <QWidget>

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

    if (!truckStockService) {
        ui.receiptsTable->setRowCount(0);
        return;
    }

    currentReceipts = truckStockService->getReceipts();

    ui.receiptsTable->setColumnCount(7);
    ui.receiptsTable->setRowCount(
        static_cast<int>(currentReceipts.size())
    );

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

        addActionButtons(row, receipt);
    }

    ui.receiptsTable->horizontalHeader()->setStretchLastSection(true);
    ui.receiptsTable->verticalHeader()->setVisible(false);

    filterReceipts();
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


void ReceiptsPage::onViewReceiptClicked(const ReceiptDto& receipt)
{
    QString details;

    details += "Receipt ID: " + QString::fromStdString(receipt.id) + "\n";
    details += "Technician: " + QString::fromStdString(receipt.technicianName) + "\n";
    details += "Truck: " + QString::fromStdString(receipt.truckNumber) + "\n";
    details += "Amount: $" + QString::number(receipt.totalAmount, 'f', 2) + "\n";
    details += "Status: " + QString::fromStdString(receipt.status) + "\n";
    details += "Created At: " + QString::fromStdString(receipt.createdAt);

    QMessageBox::information(
        this,
        "Receipt Details",
        details
    );
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

void ReceiptsPage::addActionButtons(int row, const ReceiptDto& receipt)
{
    QWidget* actionWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(actionWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    QPushButton* viewButton = new QPushButton("View", actionWidget);
    QPushButton* approveButton = new QPushButton("Approve", actionWidget);

    if (QString::fromStdString(receipt.status).compare("Approved", Qt::CaseInsensitive) == 0) {
        approveButton->setEnabled(false);
    }

    layout->addWidget(viewButton);
    layout->addWidget(approveButton);
    layout->addStretch();

    ui.receiptsTable->setCellWidget(row, 6, actionWidget);

    connect(viewButton, &QPushButton::clicked, this, [this, receipt]() {
        onViewReceiptClicked(receipt);
        });

    connect(approveButton, &QPushButton::clicked, this, [this, receipt]() {
        onApproveReceiptClicked(receipt);
        });
}

void ReceiptsPage::onViewReceiptClicked(const ReceiptDto& receipt)
{
    QString details;

    details += "Receipt ID: " + QString::fromStdString(receipt.id) + "\n";
    details += "Technician: " + QString::fromStdString(receipt.technicianName) + "\n";
    details += "Truck: " + QString::fromStdString(receipt.truckNumber) + "\n";
    details += "Total: $" + QString::number(receipt.totalAmount, 'f', 2) + "\n";
    details += "Status: " + QString::fromStdString(receipt.status) + "\n";
    details += "Date: " + QString::fromStdString(receipt.createdAt);

    QMessageBox::information(this, "Receipt Details", details);
}

void ReceiptsPage::onApproveReceiptClicked(const ReceiptDto& receipt)
{
    auto confirm = QMessageBox::question(
        this,
        "Approve Receipt",
        "Do you want to approve this receipt?"
    );

    if (confirm != QMessageBox::Yes) {
        return;
    }

    bool success = truckStockService->approveReceipt(receipt.id);

    if (success) {
        QMessageBox::information(this, "Success", "Receipt approved successfully.");
        loadReceipts();
    }
    else {
        QMessageBox::warning(this, "Error", "Failed to approve receipt. Check backend console.");
    }
}