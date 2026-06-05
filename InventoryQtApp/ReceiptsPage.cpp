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
    connect(ui.uploadButton, &QPushButton::clicked, this, &ReceiptsPage::onUploadReceiptClicked);
    connect(ui.searchInput, &QLineEdit::textChanged, this, &ReceiptsPage::onSearchChanged);

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
    ui.receiptsTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
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
    if (!truckStockService) {
        currentReceipts.clear();
        filteredReceipts.clear();
        populateTable();
        return;
    }

    currentReceipts = truckStockService->getReceipts();

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

    filteredReceipts.clear();

    for (const ReceiptDto& receipt : currentReceipts) {
        QString id = QString::fromStdString(receipt.id);
        QString technician = QString::fromStdString(receipt.technicianName);
        QString truck = QString::fromStdString(receipt.truckNumber);
        QString total = "$" + QString::number(receipt.totalAmount, 'f', 2);
        QString status = QString::fromStdString(receipt.status);
        QString date = QString::fromStdString(receipt.createdAt);

        bool searchMatch =
            searchText.isEmpty() ||
            id.contains(searchText, Qt::CaseInsensitive) ||
            technician.contains(searchText, Qt::CaseInsensitive) ||
            truck.contains(searchText, Qt::CaseInsensitive) ||
            total.contains(searchText, Qt::CaseInsensitive) ||
            status.contains(searchText, Qt::CaseInsensitive) ||
            date.contains(searchText, Qt::CaseInsensitive);

        bool statusMatch =
            selectedStatus == "All Status" ||
            status.compare(selectedStatus, Qt::CaseInsensitive) == 0;

        if (searchMatch && statusMatch) {
            filteredReceipts.push_back(receipt);
        }
    }

    populateTable();
}

void ReceiptsPage::populateTable()
{
    ui.receiptsTable->clearContents();
    ui.receiptsTable->clearSpans();

    int rowCount = static_cast<int>(filteredReceipts.size());

    if (rowCount == 0) {
        ui.receiptsTable->setRowCount(1);

        ui.receiptsTable->setItem(
            0,
            0,
            new QTableWidgetItem("No receipts found")
        );

        ui.receiptsTable->setSpan(0, 0, 1, 7);
        return;
    }

    ui.receiptsTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const ReceiptDto& receipt = filteredReceipts[row];

        ui.receiptsTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(receipt.id)));
        ui.receiptsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(receipt.technicianName)));
        ui.receiptsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(receipt.truckNumber)));
        ui.receiptsTable->setItem(row, 3, new QTableWidgetItem("$" + QString::number(receipt.totalAmount, 'f', 2)));
        ui.receiptsTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(receipt.status)));
        ui.receiptsTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(receipt.createdAt)));

        addActionButtons(row, receipt);
    }
}

void ReceiptsPage::onUploadReceiptClicked()
{
    UploadReceiptDialog dialog(truckStockService, this);

    if (dialog.exec() == QDialog::Accepted) {
        CreateReceiptRequest request;

        request.truckId = dialog.getSelectedTruckId().toStdString();
        request.fileUrl = dialog.getFilePath().toStdString();
        request.totalAmount = dialog.getTotalAmount().toDouble();

        bool success = truckStockService->createReceipt(request);

        if (success) {
            QMessageBox::information(this, "Success", "Receipt uploaded successfully.");

            emit receiptsChanged();
            loadReceipts();
        }
        else {
            QMessageBox::warning(this, "Error", "Failed to upload receipt.");
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

void ReceiptsPage::addActionButtons(
    int row,
    const ReceiptDto& receipt
)
{
    QWidget* actionWidget = new QWidget(this);
    actionWidget->setObjectName("actionContainer");

    QHBoxLayout* layout = new QHBoxLayout(actionWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->setAlignment(Qt::AlignCenter);

    QPushButton* viewButton = new QPushButton("👁", actionWidget);
    viewButton->setObjectName("viewButton");

    QPushButton* approveButton = new QPushButton("✓", actionWidget);
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

    ui.receiptsTable->setCellWidget(row, 6, actionWidget);

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
        QString fileUrl = QString::fromStdString(receipt.fileUrl);

        if (fileUrl.startsWith("http://") || fileUrl.startsWith("https://")) {
            QDesktopServices::openUrl(QUrl(fileUrl));
            return;
        }

        QDesktopServices::openUrl(QUrl::fromLocalFile(fileUrl));
        return;
    }

    QString details;

    details += "Receipt ID: " + QString::fromStdString(receipt.id) + "\n";
    details += "Technician: " + QString::fromStdString(receipt.technicianName) + "\n";
    details += "Truck: " + QString::fromStdString(receipt.truckNumber) + "\n";
    details += "Amount: $" + QString::number(receipt.totalAmount, 'f', 2) + "\n";
    details += "Status: " + QString::fromStdString(receipt.status) + "\n";
    details += "Created At: " + QString::fromStdString(receipt.createdAt);

    QMessageBox::information(this, "Receipt Details", details);
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

    bool success = truckStockService->approveReceipt(receipt.id);

    if (success) {
        QMessageBox::information(this, "Success", "Receipt approved successfully.");

        emit receiptsChanged();
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