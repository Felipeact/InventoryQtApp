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
    int lowStockCount = 0;
    int pendingReceiptCount = 0;

    if (!truckStockService) {
        ui.notificationText->setHtml(
            "<div style='color:#9ca3af;padding:8px;'>Notification service unavailable.</div>"
        );
        ui.summaryLabel->setText("Notifications unavailable");
        return;
    }

    std::vector<LowStockItemDto> lowStockItems = truckStockService->getLowStockItems();
    std::vector<ReceiptDto> receipts = truckStockService->getReceipts();

    lowStockCount = static_cast<int>(lowStockItems.size());
    for (const ReceiptDto& receipt : receipts) {
        if (QString::fromStdString(receipt.status).compare("PENDING", Qt::CaseInsensitive) == 0) {
            pendingReceiptCount++;
        }
    }

    auto esc = [](const QString& s) { return s.toHtmlEscaped(); };

    QString html =
        "<div style='font-family:Segoe UI, sans-serif; font-size:13px; color:#e5e7eb;'>";

    // --- Low stock section ---
    html += QString(
        "<div style='font-size:11px; font-weight:bold; letter-spacing:1px; color:#9ca3af; "
        "margin:4px 0 8px;'>LOW STOCK ALERTS &nbsp;"
        "<span style='background:%1; color:#fff; border-radius:8px; padding:1px 7px;'>%2</span></div>")
        .arg(lowStockCount > 0 ? "#ef4444" : "#10b981")
        .arg(lowStockCount);

    if (lowStockItems.empty()) {
        html += "<div style='color:#9ca3af; margin-bottom:14px;'>✓ Everything is stocked — no alerts.</div>";
    } else {
        const int maxItems = (std::min)(static_cast<int>(lowStockItems.size()), 8);
        for (int i = 0; i < maxItems; i++) {
            const LowStockItemDto& item = lowStockItems[i];
            const bool critical = item.currentQuantity <= 0;
            const QString dot = critical ? "#ef4444" : "#f59e0b";
            html += QString(
                "<div style='padding:5px 0; border-bottom:1px solid #2a2f3a;'>"
                "<span style='color:%1; font-weight:bold;'>●</span> "
                "<b>%2</b> &mdash; %3 "
                "<span style='color:#9ca3af;'>(%4 / %5 min · Truck %6)</span></div>")
                .arg(dot)
                .arg(esc(QString::fromStdString(item.productName)))
                .arg(critical ? "<span style='color:#ef4444;'>OUT OF STOCK</span>"
                              : "<span style='color:#f59e0b;'>low</span>")
                .arg(item.currentQuantity)
                .arg(item.minimumQuantity)
                .arg(esc(QString::fromStdString(item.truckNumber)));
        }
        if (lowStockItems.size() > 8) {
            html += QString("<div style='color:#9ca3af; padding-top:4px;'>+ %1 more…</div>")
                .arg(static_cast<int>(lowStockItems.size()) - 8);
        }
    }

    // --- Pending receipts section ---
    html += QString(
        "<div style='font-size:11px; font-weight:bold; letter-spacing:1px; color:#9ca3af; "
        "margin:16px 0 8px;'>PENDING RECEIPTS &nbsp;"
        "<span style='background:%1; color:#fff; border-radius:8px; padding:1px 7px;'>%2</span></div>")
        .arg(pendingReceiptCount > 0 ? "#3b82f6" : "#10b981")
        .arg(pendingReceiptCount);

    if (pendingReceiptCount == 0) {
        html += "<div style='color:#9ca3af;'>✓ No receipts waiting for review.</div>";
    } else {
        int shown = 0;
        for (const ReceiptDto& receipt : receipts) {
            if (QString::fromStdString(receipt.status).compare("PENDING", Qt::CaseInsensitive) != 0) {
                continue;
            }
            if (shown >= 8) break;
            shown++;
            html += QString(
                "<div style='padding:5px 0; border-bottom:1px solid #2a2f3a;'>"
                "<span style='color:#3b82f6; font-weight:bold;'>●</span> "
                "Receipt <b>%1</b> "
                "<span style='color:#9ca3af;'>· Truck %2 · $%3</span></div>")
                .arg(esc(QString::fromStdString(receipt.id)))
                .arg(esc(QString::fromStdString(receipt.truckNumber)))
                .arg(QString::number(receipt.totalAmount, 'f', 2));
        }
        if (pendingReceiptCount > 8) {
            html += QString("<div style='color:#9ca3af; padding-top:4px;'>+ %1 more…</div>")
                .arg(pendingReceiptCount - 8);
        }
    }

    html += "</div>";

    ui.notificationText->setHtml(html);

    const int total = lowStockCount + pendingReceiptCount;
    ui.summaryLabel->setText(
        total == 0
            ? QStringLiteral("You're all caught up")
            : QString("%1 low stock • %2 pending receipts")
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