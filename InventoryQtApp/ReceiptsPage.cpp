#include "ReceiptsPage.h"
#include "Theme.h"
#include "UploadReceiptDialog.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QComboBox>
#include <QDesktopServices>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QUrl>
#include <QWidget>

ReceiptsPage::ReceiptsPage(
    TruckStockService* truckStockService,
    const std::vector<std::string>& permissions,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService),
    permissions(permissions)
{
    ui.setupUi(this);

    applyTheme(Theme::AppTheme::Dark);

    setupTable();
    setupConnections();
    loadReceipts();
}

ReceiptsPage::~ReceiptsPage()
{
}

void ReceiptsPage::setupConnections()
{
    connect(
        ui.uploadButton,
        &QPushButton::clicked,
        this,
        &ReceiptsPage::onUploadReceiptClicked
    );

    connect(
        ui.searchInput,
        &QLineEdit::textChanged,
        this,
        &ReceiptsPage::onSearchChanged
    );

    connect(
        ui.statusFilter,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &ReceiptsPage::onStatusFilterChanged
    );
}

void ReceiptsPage::setupTable()
{
    ui.receiptsTable->setColumnCount(7);

    ui.receiptsTable->setHorizontalHeaderLabels(
        QStringList()
        << "Receipt #"
        << "Technician"
        << "Truck"
        << "Total"
        << "Status"
        << "Date"
        << "Actions"
    );

    ui.receiptsTable->verticalHeader()->setVisible(false);
    ui.receiptsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.receiptsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.receiptsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.receiptsTable->setShowGrid(false);
    ui.receiptsTable->setFrameShape(QFrame::NoFrame);
    ui.receiptsTable->setFocusPolicy(Qt::NoFocus);
    ui.receiptsTable->setAlternatingRowColors(false);
    ui.receiptsTable->viewport()->setAutoFillBackground(false);

    ui.receiptsTable->horizontalHeader()->setHighlightSections(false);
    ui.receiptsTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    ui.receiptsTable->horizontalHeader()->setFixedHeight(48);
    ui.receiptsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.receiptsTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);

    ui.receiptsTable->setColumnWidth(6, 150);
    ui.receiptsTable->verticalHeader()->setDefaultSectionSize(52);

    ui.receiptsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.receiptsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void ReceiptsPage::loadReceipts()
{
    ui.receiptsTable->clearContents();

    if (!truckStockService) {
        ui.receiptsTable->setRowCount(0);
        return;
    }

    currentReceipts =
        truckStockService->getReceipts();

    ui.receiptsTable->setRowCount(
        static_cast<int>(currentReceipts.size())
    );

    for (int row = 0; row < static_cast<int>(currentReceipts.size()); row++) {
        const ReceiptDto& receipt =
            currentReceipts[row];

        ui.receiptsTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::fromStdString(receipt.id))
        );

        ui.receiptsTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(receipt.technicianName))
        );

        ui.receiptsTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::fromStdString(receipt.truckNumber))
        );

        ui.receiptsTable->setItem(
            row,
            3,
            new QTableWidgetItem("$" + QString::number(receipt.totalAmount, 'f', 2))
        );

        ui.receiptsTable->setItem(
            row,
            4,
            new QTableWidgetItem(QString::fromStdString(receipt.status))
        );

        ui.receiptsTable->setItem(
            row,
            5,
            new QTableWidgetItem(QString::fromStdString(receipt.createdAt))
        );

        addActionButtons(
            row,
            receipt
        );
    }

    filterReceipts();
}

void ReceiptsPage::refreshReceipts()
{
    loadReceipts();
}

void ReceiptsPage::filterReceipts()
{
    QString searchText =
        ui.searchInput->text().trimmed();

    QString selectedStatus =
        ui.statusFilter->currentText();

    for (int row = 0; row < ui.receiptsTable->rowCount(); row++) {
        bool searchMatch =
            searchText.isEmpty();

        bool statusMatch =
            selectedStatus == "All Status";

        for (int col = 0; col < ui.receiptsTable->columnCount(); col++) {
            QTableWidgetItem* item =
                ui.receiptsTable->item(row, col);

            if (item && item->text().contains(searchText, Qt::CaseInsensitive)) {
                searchMatch = true;
            }
        }

        QTableWidgetItem* statusItem =
            ui.receiptsTable->item(row, 4);

        if (
            statusItem &&
            statusItem->text().compare(selectedStatus, Qt::CaseInsensitive) == 0
            ) {
            statusMatch = true;
        }

        ui.receiptsTable->setRowHidden(
            row,
            !(searchMatch && statusMatch)
        );
    }
}

void ReceiptsPage::onUploadReceiptClicked()
{
    UploadReceiptDialog dialog(
        truckStockService,
        this
    );

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

void ReceiptsPage::onSearchChanged(
    const QString& text
)
{
    Q_UNUSED(text);

    filterReceipts();
}

void ReceiptsPage::onStatusFilterChanged(
    int index
)
{
    Q_UNUSED(index);

    filterReceipts();
}

void ReceiptsPage::addActionButtons(
    int row,
    const ReceiptDto& receipt
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

    QPushButton* viewButton =
        new QPushButton("👁", actionWidget);

    viewButton->setObjectName("viewButton");

    QPushButton* approveButton =
        new QPushButton("✓", actionWidget);

    approveButton->setObjectName("approveButton");

    if (!hasPermission("APPROVE_RECEIPTS")) {
        approveButton->hide();
    }

    if (
        QString::fromStdString(receipt.status)
        .compare("APPROVED", Qt::CaseInsensitive) == 0
        ) {
        approveButton->setEnabled(false);
    }

    layout->addWidget(viewButton);
    layout->addWidget(approveButton);

    ui.receiptsTable->setCellWidget(
        row,
        6,
        actionWidget
    );

    connect(viewButton, &QPushButton::clicked, this, [this, receipt]() {
        onViewReceiptClicked(receipt);
        });

    connect(approveButton, &QPushButton::clicked, this, [this, receipt]() {
        onApproveReceiptClicked(receipt);
        });
}

void ReceiptsPage::onViewReceiptClicked(
    const ReceiptDto& receipt
)
{
    if (!receipt.fileUrl.empty()) {
        QString fileUrl =
            QString::fromStdString(receipt.fileUrl);

        if (
            fileUrl.startsWith("http://") ||
            fileUrl.startsWith("https://")
            ) {
            QDesktopServices::openUrl(QUrl(fileUrl));
            return;
        }

        QDesktopServices::openUrl(
            QUrl::fromLocalFile(fileUrl)
        );

        return;
    }

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

void ReceiptsPage::onApproveReceiptClicked(
    const ReceiptDto& receipt
)
{
    QMessageBox::StandardButton confirm =
        QMessageBox::question(
            this,
            "Approve Receipt",
            "Do you want to approve this receipt?",
            QMessageBox::Yes | QMessageBox::No
        );

    if (confirm != QMessageBox::Yes) {
        return;
    }

    bool success =
        truckStockService->approveReceipt(receipt.id);

    if (success) {
        QMessageBox::information(
            this,
            "Success",
            "Receipt approved successfully."
        );

        loadReceipts();
    }
    else {
        QMessageBox::warning(
            this,
            "Error",
            "Failed to approve receipt. Check backend console."
        );
    }
}

bool ReceiptsPage::hasPermission(
    const std::string& permission
) const
{
    return std::find(
        permissions.begin(),
        permissions.end(),
        permission
    ) != permissions.end();
}

void ReceiptsPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::receiptsPageStyle(theme)
    );
}