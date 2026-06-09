#include "TruckStockDashboardPage.h"
#include "Theme.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QDateTime>
#include <QFrame>
#include <QHeaderView>
#include <QMetaObject>
#include <QPointer>
#include <QPushButton>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QThread>
#include <QTimer>

namespace
{
    QString formatIsoDateTime(
        const std::string& value
    )
    {
        QString text =
            QString::fromStdString(value).trimmed();

        if (text.isEmpty()) {
            return "N/A";
        }

        QDateTime dateTime =
            QDateTime::fromString(
                text,
                Qt::ISODate
            );

        if (dateTime.isValid()) {
            return dateTime
                .toLocalTime()
                .toString("yyyy-MM-dd HH:mm");
        }

        text.replace("T", " ");

        int dotIndex =
            text.indexOf('.');

        if (dotIndex >= 0) {
            text =
                text.left(dotIndex);
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

TruckStockDashboardPage::TruckStockDashboardPage(
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);

    applyTheme(Theme::AppTheme::Dark);

    setupTables();
    setupConnections();

    showLoadingState();

    QTimer::singleShot(
        0,
        this,
        [this]() {
            loadDashboardData();
        }
    );
}

TruckStockDashboardPage::~TruckStockDashboardPage()
{
}

void TruckStockDashboardPage::setupConnections()
{
    connect(
        ui.viewAllTrucksButton,
        &QPushButton::clicked,
        this,
        [this]() {
            emit viewAllTrucksRequested();
        }
    );

    connect(
        ui.viewAllLowStockButton,
        &QPushButton::clicked,
        this,
        [this]() {
            emit viewAllLowStockRequested();
        }
    );
}

void TruckStockDashboardPage::setupTables()
{
    ui.recentTrucksTable->setColumnCount(5);

    ui.recentTrucksTable->setHorizontalHeaderLabels(
        QStringList()
        << "Truck"
        << "Plate"
        << "Technician"
        << "Status"
        << "Updated"
    );

    ui.lowStockTable->setColumnCount(4);

    ui.lowStockTable->setHorizontalHeaderLabels(
        QStringList()
        << "Truck"
        << "Item"
        << "Current"
        << "Minimum"
    );

    QList<QTableWidget*> tables = {
        ui.recentTrucksTable,
        ui.lowStockTable
    };

    for (QTableWidget* table : tables) {
        table->verticalHeader()->setVisible(false);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setShowGrid(false);
        table->setFrameShape(QFrame::NoFrame);
        table->setFocusPolicy(Qt::NoFocus);
        table->setAlternatingRowColors(false);
        table->viewport()->setAutoFillBackground(false);

        table->horizontalHeader()->setHighlightSections(false);

        table->horizontalHeader()->setDefaultAlignment(
            Qt::AlignLeft | Qt::AlignVCenter
        );

        table->horizontalHeader()->setFixedHeight(42);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        table->verticalHeader()->setDefaultSectionSize(42);

        table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}

void TruckStockDashboardPage::showLoadingState()
{
    ui.metricTitle1->setText("Total Trucks");

    ui.metricValue1->setText(
        hasCachedData
        ? QString::number(static_cast<int>(cachedData.trucks.size()))
        : "..."
    );

    ui.positiveLabel1->setText(
        hasCachedData
        ? "Cached"
        : "Loading"
    );

    ui.metricTitle2->setText("Stock Templates");

    ui.metricValue2->setText(
        hasCachedData
        ? QString::number(static_cast<int>(cachedData.templates.size()))
        : "..."
    );

    ui.positiveLabel2->setText(
        hasCachedData
        ? "Cached"
        : "Loading"
    );

    ui.metricTitle3->setText("Active Trucks");
    ui.metricValue3->setText("...");
    ui.positiveLabel3->setText("Loading");

    ui.metricTitle4->setText("Low Stock Items");

    ui.metricValue4->setText(
        hasCachedData
        ? QString::number(static_cast<int>(cachedData.lowStockItems.size()))
        : "..."
    );

    ui.negativeLabel1->setText(
        hasCachedData
        ? "Cached"
        : "Loading"
    );

    if (!hasCachedData) {
        ui.recentTrucksTable->setRowCount(1);

        ui.recentTrucksTable->setItem(
            0,
            0,
            new QTableWidgetItem("Loading trucks...")
        );

        ui.recentTrucksTable->setSpan(0, 0, 1, 5);

        ui.lowStockTable->setRowCount(1);

        ui.lowStockTable->setItem(
            0,
            0,
            new QTableWidgetItem("Loading low stock items...")
        );

        ui.lowStockTable->setSpan(0, 0, 1, 4);
    }
}

void TruckStockDashboardPage::loadDashboardData()
{
    if (!truckStockService) {
        return;
    }

    if (refreshInProgress) {
        pendingRefresh = true;
        return;
    }

    refreshInProgress = true;
    pendingRefresh = false;

    showLoadingState();

    TruckStockService* service =
        truckStockService;

    QPointer<TruckStockDashboardPage> self(this);

    QThread* worker =
        QThread::create(
            [self, service]() {
                DashboardData data;

                data.trucks =
                    service->getTrucks();

                data.templates =
                    service->getTemplates();

                data.lowStockItems =
                    service->getLowStockItems();

                QMetaObject::invokeMethod(
                    self,
                    [self, data]() {
                        if (!self) {
                            return;
                        }

                        self->refreshInProgress = false;
                        self->cachedData = data;
                        self->hasCachedData = true;

                        self->applyDashboardData(data);

                        if (self->pendingRefresh) {
                            self->pendingRefresh = false;
                            self->loadDashboardData();
                        }
                    },
                    Qt::QueuedConnection
                );
            }
        );

    connect(
        worker,
        &QThread::finished,
        worker,
        &QObject::deleteLater
    );

    worker->start();
}

void TruckStockDashboardPage::refreshDashboard()
{
    loadDashboardData();
}

void TruckStockDashboardPage::applyDashboardData(
    const DashboardData& data
)
{
    populateMetricCards(
        data.trucks,
        data.templates,
        data.lowStockItems
    );

    populateRecentTrucks(data.trucks);
    populateLowStockItems(data.lowStockItems);
}

void TruckStockDashboardPage::populateRecentTrucks(
    const std::vector<TruckDto>& trucks
)
{
    ui.recentTrucksTable->setUpdatesEnabled(false);
    ui.recentTrucksTable->clearContents();
    ui.recentTrucksTable->clearSpans();

    int rowCount =
        (std::min)(
            static_cast<int>(trucks.size()),
            5
            );

    if (rowCount == 0) {
        ui.recentTrucksTable->setRowCount(1);

        ui.recentTrucksTable->setItem(
            0,
            0,
            new QTableWidgetItem("No trucks found")
        );

        ui.recentTrucksTable->setSpan(0, 0, 1, 5);
        ui.recentTrucksTable->setUpdatesEnabled(true);

        return;
    }

    ui.recentTrucksTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const TruckDto& truck =
            trucks[row];

        ui.recentTrucksTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::fromStdString(truck.truckName))
        );

        ui.recentTrucksTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(truck.licensePlate))
        );

        ui.recentTrucksTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::fromStdString(truck.technicianName))
        );

        ui.recentTrucksTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::fromStdString(truck.status))
        );

        ui.recentTrucksTable->setItem(
            row,
            4,
            new QTableWidgetItem(formatIsoDateTime(truck.updatedAt))
        );
    }

    ui.recentTrucksTable->setUpdatesEnabled(true);
}

void TruckStockDashboardPage::populateLowStockItems(
    const std::vector<LowStockItemDto>& lowStockItems
)
{
    ui.lowStockTable->setUpdatesEnabled(false);
    ui.lowStockTable->clearContents();
    ui.lowStockTable->clearSpans();

    int rowCount =
        (std::min)(
            static_cast<int>(lowStockItems.size()),
            5
            );

    if (rowCount == 0) {
        ui.lowStockTable->setRowCount(1);

        ui.lowStockTable->setItem(
            0,
            0,
            new QTableWidgetItem("No low stock truck items")
        );

        ui.lowStockTable->setSpan(0, 0, 1, 4);
        ui.lowStockTable->setUpdatesEnabled(true);

        return;
    }

    ui.lowStockTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const LowStockItemDto& item =
            lowStockItems[row];

        ui.lowStockTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::fromStdString(item.truckNumber))
        );

        ui.lowStockTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(item.productName))
        );

        ui.lowStockTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::number(item.currentQuantity))
        );

        ui.lowStockTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::number(item.minimumQuantity))
        );
    }

    ui.lowStockTable->setUpdatesEnabled(true);
}

void TruckStockDashboardPage::populateMetricCards(
    const std::vector<TruckDto>& trucks,
    const std::vector<StockTemplateDto>& templates,
    const std::vector<LowStockItemDto>& lowStockItems
)
{
    int totalTrucks =
        static_cast<int>(trucks.size());

    int totalTemplates =
        static_cast<int>(templates.size());

    int lowStockCount =
        static_cast<int>(lowStockItems.size());

    int activeTrucks = 0;

    for (const TruckDto& truck : trucks) {
        QString status =
            QString::fromStdString(truck.status).toLower();

        if (status == "active") {
            activeTrucks++;
        }
    }

    ui.metricTitle1->setText("Total Trucks");
    ui.metricValue1->setText(QString::number(totalTrucks));
    ui.positiveLabel1->setText("Trucks registered");

    ui.metricTitle2->setText("Stock Templates");
    ui.metricValue2->setText(QString::number(totalTemplates));
    ui.positiveLabel2->setText("Templates available");

    ui.metricTitle3->setText("Active Trucks");
    ui.metricValue3->setText(QString::number(activeTrucks));
    ui.positiveLabel3->setText("Currently active");

    ui.metricTitle4->setText("Low Stock Items");
    ui.metricValue4->setText(QString::number(lowStockCount));
    ui.negativeLabel1->setText("Need attention");

    ui.summarySmallLabel1->setText("Total Trucks");
    ui.summaryValue1->setText(QString::number(totalTrucks));
    ui.summaryPositive1->setText("Registered");

    ui.summarySmallLabel2->setText("Templates");
    ui.summaryValue2->setText(QString::number(totalTemplates));
    ui.summaryNegative1->setText("Available");

    ui.summarySmallLabel3->setText("Active Trucks");
    ui.summaryValue3->setText(QString::number(activeTrucks));
    ui.summaryPositive2->setText("Active");

    ui.summarySmallLabel4->setText("Low Stock");
    ui.summaryValue4->setText(QString::number(lowStockCount));
    ui.summaryNegative2->setText("Alerts");
}

void TruckStockDashboardPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::truckStockDashboardPageStyle(theme)
    );
}