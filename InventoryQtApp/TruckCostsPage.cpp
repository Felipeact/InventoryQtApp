#include "TruckCostsPage.h"

#include <algorithm>
#include <cctype>
#include <map>

#include <QAbstractItemView>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

TruckCostsPage::TruckCostsPage(
    TruckStockService* truckStockService,
    const std::vector<std::string>& permissions,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService),
    permissions(permissions)
{
    buildUi();
    applyTheme(Theme::AppTheme::Dark);
}

QString TruckCostsPage::money(double value)
{
    return "$" + QString::number(value, 'f', 2);
}

void TruckCostsPage::buildUi()
{
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(16);

    QLabel* title = new QLabel("Truck Costs", this);
    title->setObjectName("pageTitle");
    title->setStyleSheet("font-size: 22px; font-weight: 800;");
    root->addWidget(title);

    QLabel* subtitle = new QLabel(
        "Spending per truck from uploaded receipts. Rejected receipts are "
        "excluded.",
        this
    );
    subtitle->setObjectName("pageSubtitle");
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    // Summary cards row.
    QHBoxLayout* cards = new QHBoxLayout();
    cards->setSpacing(14);

    auto makeCard = [this](const QString& caption, QLabel*& valueOut) {
        QFrame* card = new QFrame(this);
        card->setObjectName("summaryCard");
        card->setFrameShape(QFrame::StyledPanel);

        QVBoxLayout* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(18, 14, 18, 14);
        cardLayout->setSpacing(4);

        QLabel* cap = new QLabel(caption, card);
        cap->setObjectName("summaryCaption");

        valueOut = new QLabel("-", card);
        valueOut->setObjectName("summaryValue");
        valueOut->setStyleSheet("font-size: 20px; font-weight: 800;");

        cardLayout->addWidget(cap);
        cardLayout->addWidget(valueOut);
        return card;
    };

    cards->addWidget(makeCard("Fleet spend", fleetSpendValue));
    cards->addWidget(makeCard("Receipts counted", receiptsCountValue));
    cards->addStretch();
    root->addLayout(cards);

    // Rejected banner (hidden unless there are rejected receipts).
    rejectedBanner = new QFrame(this);
    rejectedBanner->setObjectName("rejectedBanner");
    rejectedBanner->setFrameShape(QFrame::StyledPanel);
    QHBoxLayout* bannerLayout = new QHBoxLayout(rejectedBanner);
    bannerLayout->setContentsMargins(14, 10, 14, 10);
    rejectedLabel = new QLabel("", rejectedBanner);
    rejectedLabel->setWordWrap(true);
    bannerLayout->addWidget(rejectedLabel);
    rejectedBanner->setVisible(false);
    root->addWidget(rejectedBanner);

    QLabel* tableTitle = new QLabel("Spend by truck", this);
    tableTitle->setStyleSheet("font-weight: 700;");
    root->addWidget(tableTitle);

    table = new QTableWidget(this);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(
        QStringList() << "Truck" << "Receipts" << "Spend"
    );
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    root->addWidget(table, 1);
}

void TruckCostsPage::refreshCosts()
{
    if (!truckStockService) {
        return;
    }

    std::vector<ReceiptDto> receipts = truckStockService->getReceipts();
    populate(receipts);
}

namespace {
    bool isRejected(const ReceiptDto& r)
    {
        std::string status = r.status;
        std::transform(
            status.begin(), status.end(), status.begin(),
            [](unsigned char c) { return std::toupper(c); }
        );
        return status == "REJECTED";
    }
}

void TruckCostsPage::populate(const std::vector<ReceiptDto>& receipts)
{
    struct Spend {
        double total = 0.0;
        int count = 0;
    };

    std::map<std::string, Spend> byTruck;
    double fleetTotal = 0.0;
    int countedReceipts = 0;
    double rejectedTotal = 0.0;
    int rejectedCount = 0;

    for (const ReceiptDto& r : receipts) {
        if (isRejected(r)) {
            rejectedTotal += r.totalAmount;
            rejectedCount += 1;
            continue;
        }

        std::string label = r.truckNumber.empty()
            ? std::string("Unassigned")
            : ("Truck " + r.truckNumber);

        Spend& s = byTruck[label];
        s.total += r.totalAmount;
        s.count += 1;

        fleetTotal += r.totalAmount;
        countedReceipts += 1;
    }

    fleetSpendValue->setText(money(fleetTotal));
    receiptsCountValue->setText(QString::number(countedReceipts));

    if (rejectedCount > 0) {
        rejectedLabel->setText(
            money(rejectedTotal) +
            QString(" across %1 rejected receipt%2 — excluded from costs.")
                .arg(rejectedCount)
                .arg(rejectedCount == 1 ? "" : "s")
        );
        rejectedBanner->setVisible(true);
    } else {
        rejectedBanner->setVisible(false);
    }

    // Sort trucks by spend, descending.
    std::vector<std::pair<std::string, Spend>> rows(
        byTruck.begin(), byTruck.end()
    );
    std::sort(
        rows.begin(), rows.end(),
        [](const std::pair<std::string, Spend>& a,
           const std::pair<std::string, Spend>& b) {
            return a.second.total > b.second.total;
        }
    );

    table->setRowCount(static_cast<int>(rows.size()));
    for (int i = 0; i < static_cast<int>(rows.size()); ++i) {
        const std::string& label = rows[i].first;
        const Spend& s = rows[i].second;

        table->setItem(
            i, 0,
            new QTableWidgetItem(QString::fromStdString(label))
        );
        table->setItem(
            i, 1,
            new QTableWidgetItem(QString::number(s.count))
        );

        QTableWidgetItem* spendItem = new QTableWidgetItem(money(s.total));
        spendItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        table->setItem(i, 2, spendItem);
    }
}

void TruckCostsPage::applyTheme(Theme::AppTheme theme)
{
    currentTheme = theme;
    // Reuse the receipts page styling — Truck Costs is its analytical sibling.
    setStyleSheet(Theme::receiptsPageStyle(theme));
}
