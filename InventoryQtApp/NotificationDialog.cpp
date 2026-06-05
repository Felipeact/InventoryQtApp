#include "NotificationDialog.h"

#include <QPushButton>
#include <QTextEdit>

#include <algorithm>

NotificationDialog::NotificationDialog(
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QDialog(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);

    setModal(true);

    setupConnections();
    loadNotifications();

    applyTheme(Theme::AppTheme::Dark);
}

NotificationDialog::~NotificationDialog()
{
}

void NotificationDialog::setupConnections()
{
    connect(
        ui.openLowStockButton,
        &QPushButton::clicked,
        this,
        &NotificationDialog::onOpenLowStockClicked
    );

    connect(
        ui.openReceiptsButton,
        &QPushButton::clicked,
        this,
        &NotificationDialog::onOpenReceiptsClicked
    );

    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &NotificationDialog::onCloseClicked
    );
}

void NotificationDialog::loadNotifications()
{
    QString content;

    int lowStockCount = 0;
    int pendingReceiptCount = 0;

    if (truckStockService) {
        std::vector<LowStockItemDto> lowStockItems =
            truckStockService->getLowStockItems();

        std::vector<ReceiptDto> receipts =
            truckStockService->getReceipts();

        lowStockCount =
            static_cast<int>(lowStockItems.size());

        content += "LOW STOCK ALERTS (" + QString::number(lowStockCount) + ")\n\n";

        if (lowStockItems.empty()) {
            content += "No low stock alerts.\n\n";
        }
        else {
            int maxItems =
                (std::min)(static_cast<int>(lowStockItems.size()), 8);

            for (int i = 0; i < maxItems; i++) {
                const LowStockItemDto& item = lowStockItems[i];

                content += "⚠ Truck "
                    + QString::fromStdString(item.truckNumber)
                    + " - "
                    + QString::fromStdString(item.productName)
                    + " ("
                    + QString::number(item.currentQuantity)
                    + "/"
                    + QString::number(item.minimumQuantity)
                    + ")\n";
            }

            if (lowStockItems.size() > 8) {
                content += "...and more\n";
            }

            content += "\n";
        }

        content += "PENDING RECEIPTS\n\n";

        for (const ReceiptDto& receipt : receipts) {
            QString status =
                QString::fromStdString(receipt.status);

            if (status.compare("PENDING", Qt::CaseInsensitive) == 0) {
                pendingReceiptCount++;

                if (pendingReceiptCount <= 8) {
                    content += "• Receipt "
                        + QString::fromStdString(receipt.id)
                        + " - "
                        + QString::fromStdString(receipt.truckNumber)
                        + " - $"
                        + QString::number(receipt.totalAmount, 'f', 2)
                        + "\n";
                }
            }
        }

        if (pendingReceiptCount == 0) {
            content += "No pending receipts.\n";
        }
        else if (pendingReceiptCount > 8) {
            content += "...and more\n";
        }
    }
    else {
        content = "Notification service unavailable.";
    }

    ui.notificationText->setPlainText(content);

    ui.summaryLabel->setText(
        QString("%1 low stock alerts • %2 pending receipts")
        .arg(lowStockCount)
        .arg(pendingReceiptCount)
    );
}

void NotificationDialog::onOpenLowStockClicked()
{
    emit openLowStockRequested();
    accept();
}

void NotificationDialog::onOpenReceiptsClicked()
{
    emit openReceiptsRequested();
    accept();
}

void NotificationDialog::onCloseClicked()
{
    reject();
}

void NotificationDialog::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::notificationDialogStyle(theme)
    );
}