#include "ReceiptsPage.h"
#include "IconUtil.h"
#include "Theme.h"
#include "UploadReceiptDialog.h"
#include "ReceiptDetailsDialog.h"

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

namespace
{
    QString formatIsoDateTime(const std::string& value)
    {
        QString text = QString::fromStdString(value).trimmed();

        if (text.isEmpty()) {
            return "N/A";
        }

        text.replace("T", " ");

        int dotIndex = text.indexOf('.');
        if (dotIndex >= 0) {
            text = text.left(dotIndex);
        }

        if (text.endsWith("Z")) {
            text.chop(1);
        }

        if (text.length() >= 16) {
            return text.left(16);
        }

        return text;
    }
}
#include <QThread>
#include <QPointer>
#include <QMetaObject>

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

    ui.receiptsTable->setColumnWidth(6, 190);
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

    QPointer<ReceiptsPage> self(this);
    TruckStockService* service = truckStockService;

    QThread* worker =
        QThread::create([self, service]() {
        std::vector<ReceiptDto> receipts =
            service->getReceipts();

        if (!self) {
            return;
        }

        QMetaObject::invokeMethod(
            self,
            [self, receipts]() {
                if (!self) {
                    return;
                }

                self->currentReceipts = receipts;
                self->filterReceipts();
            },
            Qt::QueuedConnection
        );
            });

    connect(
        worker,
        &QThread::finished,
        worker,
        &QObject::deleteLater
    );

    worker->start();
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

    filteredReceipts.clear();

    for (const ReceiptDto& receipt : currentReceipts) {
        QString id =
            QString::fromStdString(receipt.id);

        QString technician =
            QString::fromStdString(receipt.technicianName);

        QString truck =
            QString::fromStdString(receipt.truckNumber);

        QString total =
            "$" + QString::number(receipt.totalAmount, 'f', 2);

        QString status =
            QString::fromStdString(receipt.status);

        QString date =
            QString::fromStdString(receipt.createdAt);

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
    ui.receiptsTable->setUpdatesEnabled(false);
    ui.receiptsTable->clearContents();
    ui.receiptsTable->clearSpans();

    int rowCount =
        static_cast<int>(filteredReceipts.size());

    if (rowCount == 0) {
        ui.receiptsTable->setRowCount(1);

        ui.receiptsTable->setItem(
            0,
            0,
            new QTableWidgetItem("No receipts found")
        );

        ui.receiptsTable->setSpan(0, 0, 1, 7);
        ui.receiptsTable->setUpdatesEnabled(true);
        return;
    }

    ui.receiptsTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const ReceiptDto& receipt =
            filteredReceipts[row];

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
            new QTableWidgetItem(formatIsoDateTime(receipt.createdAt))
        );

        addActionButtons(
            row,
            receipt
        );
    }

    ui.receiptsTable->setUpdatesEnabled(true);
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

            emit receiptsChanged();
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
    IconUtil::iconifyActionButton(viewButton);

    QPushButton* approveButton =
        new QPushButton("✓", actionWidget);

    approveButton->setObjectName("approveButton");

    QPushButton* rejectButton =
        new QPushButton("✕", actionWidget);

    rejectButton->setObjectName("rejectButton");

    bool canApproveOrReject =
        hasPermission("APPROVE_RECEIPTS") ||
        hasPermission("REJECT_RECEIPTS");

    if (!canApproveOrReject) {
        approveButton->hide();
        rejectButton->hide();
    }

    QString receiptStatus =
        QString::fromStdString(receipt.status);

    if (
        receiptStatus.compare("APPROVED", Qt::CaseInsensitive) == 0 ||
        receiptStatus.compare("REJECTED", Qt::CaseInsensitive) == 0
        ) {
        approveButton->setEnabled(false);
        rejectButton->setEnabled(false);
    }

    layout->addWidget(viewButton);
    layout->addWidget(approveButton);
    layout->addWidget(rejectButton);

    ui.receiptsTable->setCellWidget(
        row,
        6,
        actionWidget
    );

    connect(
        viewButton,
        &QPushButton::clicked,
        this,
        [this, receipt]() {
            onViewReceiptClicked(receipt);
        }
    );

    connect(
        approveButton,
        &QPushButton::clicked,
        this,
        [this, receipt]() {
            onApproveReceiptClicked(receipt);
        }
    );

    connect(
        rejectButton,
        &QPushButton::clicked,
        this,
        [this, receipt]() {
            onRejectReceiptClicked(receipt);
        }
    );
}

void ReceiptsPage::onViewReceiptClicked(
    const ReceiptDto& receipt
)
{
    ReceiptDetailsDialog dialog(
        receipt,
        this
    );

    dialog.exec();
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

void ReceiptsPage::onRejectReceiptClicked(
    const ReceiptDto& receipt
)
{
    QMessageBox::StandardButton confirm =
        QMessageBox::question(
            this,
            "Reject Receipt",
            "Do you want to reject this receipt?",
            QMessageBox::Yes | QMessageBox::No
        );

    if (confirm != QMessageBox::Yes) {
        return;
    }

    bool success =
        truckStockService->rejectReceipt(receipt.id);

    if (success) {
        QMessageBox::information(
            this,
            "Success",
            "Receipt rejected successfully."
        );

        emit receiptsChanged();
        loadReceipts();
    }
    else {
        QMessageBox::warning(
            this,
            "Error",
            "Failed to reject receipt. Check backend console."
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

void ReceiptsPage::setSearchText(
    const QString& text
)
{
    ui.searchInput->setText(text);
}