#include "LowStockAlertsPage.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>

LowStockAlertsPage::LowStockAlertsPage(
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);
    setupConnections();
    loadAlerts();
}

LowStockAlertsPage::~LowStockAlertsPage()
{
}

void LowStockAlertsPage::setupConnections()
{
    connect(ui.searchInput, &QLineEdit::textChanged,
        this, &LowStockAlertsPage::onSearchChanged);

    connect(ui.statusFilter,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &LowStockAlertsPage::onStatusFilterChanged);

    connect(ui.exportButton, &QPushButton::clicked,
        this, &LowStockAlertsPage::onExportClicked);
}

void LowStockAlertsPage::loadAlerts()
{
    ui.alertsTable->clearContents();

    currentAlerts = truckStockService->getLowStockItems();

    ui.alertsTable->setRowCount(
        static_cast<int>(currentAlerts.size())
    );

    ui.alertsTable->setColumnCount(6);

    int criticalCount = 0;
    int warningCount = 0;

    for (int row = 0; row < static_cast<int>(currentAlerts.size()); ++row) {
        const LowStockItemDto& item = currentAlerts[row];

        if (item.status == "Critical") {
            criticalCount++;
        }
        else {
            warningCount++;
        }

        ui.alertsTable->setItem(row, 0,
            new QTableWidgetItem(QString::fromStdString(item.truckNumber)));

        ui.alertsTable->setItem(row, 1,
            new QTableWidgetItem(QString::fromStdString(item.productName)));

        ui.alertsTable->setItem(row, 2,
            new QTableWidgetItem(QString::number(item.currentQuantity)));

        ui.alertsTable->setItem(row, 3,
            new QTableWidgetItem(QString::number(item.minimumQuantity)));

        ui.alertsTable->setItem(row, 4,
            new QTableWidgetItem(QString::fromStdString(item.status)));

        ui.alertsTable->setItem(row, 5,
            new QTableWidgetItem(QString::fromStdString(item.templateName)));
    }

    ui.metricValue->setText(QString::number(criticalCount));
    ui.metricValue1->setText(QString::number(warningCount));
    ui.metricValue2->setText(QString::number(
        currentAlerts.size() - criticalCount - warningCount
    ));

    ui.alertsTable->horizontalHeader()->setStretchLastSection(true);
    ui.alertsTable->verticalHeader()->setVisible(false);

    ui.paginationLabel->setText(
        QString("Showing %1 low stock alerts")
        .arg(currentAlerts.size())
    );
}

void LowStockAlertsPage::refreshAlerts()
{
    loadAlerts();
}

void LowStockAlertsPage::filterAlerts()
{
    QString searchText = ui.searchInput->text().trimmed();
    QString selectedStatus = ui.statusFilter->currentText();

    for (int row = 0; row < ui.alertsTable->rowCount(); ++row) {
        bool searchMatch = searchText.isEmpty();
        bool statusMatch =
            selectedStatus == "All Alerts";

        for (int col = 0; col < ui.alertsTable->columnCount(); ++col) {
            QTableWidgetItem* item = ui.alertsTable->item(row, col);

            if (item && item->text().contains(searchText, Qt::CaseInsensitive)) {
                searchMatch = true;
            }
        }

        QTableWidgetItem* statusItem = ui.alertsTable->item(row, 4);

        if (statusItem && statusItem->text() == selectedStatus) {
            statusMatch = true;
        }

        ui.alertsTable->setRowHidden(
            row,
            !(searchMatch && statusMatch)
        );
    }
}

void LowStockAlertsPage::onSearchChanged(const QString& text)
{
    Q_UNUSED(text);
    filterAlerts();
}

void LowStockAlertsPage::onStatusFilterChanged(int index)
{
    Q_UNUSED(index);
    filterAlerts();
}

void LowStockAlertsPage::onExportClicked()
{
    QMessageBox::information(
        this,
        "Export",
        "Export report will be added later."
    );
}