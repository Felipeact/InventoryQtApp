#include "LowStockAlertsPage.h"
#include "Theme.h"

#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextStream>
#include <algorithm>

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

    connect(ui.pageButton, &QPushButton::clicked,
        this, &LowStockAlertsPage::onPreviousPageClicked);

    connect(ui.pageButton3, &QPushButton::clicked,
        this, &LowStockAlertsPage::onNextPageClicked);

    connect(ui.pageButton2, &QPushButton::clicked,
        this, &LowStockAlertsPage::onPage2Clicked);
}

void LowStockAlertsPage::loadAlerts()
{
    ui.alertsTable->clearContents();
    ui.alertsTable->setColumnCount(6);

    if (!truckStockService) {
        currentAlerts.clear();
        filteredAlerts.clear();
        populateTable();
        updateMetrics();
        updatePagination();
        return;
    }

    currentAlerts = truckStockService->getLowStockItems();

    filterAlerts();

    ui.alertsTable->horizontalHeader()->setStretchLastSection(true);
    ui.alertsTable->verticalHeader()->setVisible(false);
}

void LowStockAlertsPage::refreshAlerts()
{
    loadAlerts();
}

void LowStockAlertsPage::filterAlerts()
{
    QString searchText = ui.searchInput->text().trimmed();
    QString selectedStatus = ui.statusFilter->currentText();

    filteredAlerts.clear();

    for (const LowStockItemDto& item : currentAlerts) {
        QString truckNumber = QString::fromStdString(item.truckNumber);
        QString productName = QString::fromStdString(item.productName);
        QString currentQuantity = QString::number(item.currentQuantity);
        QString minimumQuantity = QString::number(item.minimumQuantity);
        QString status = QString::fromStdString(item.status);
        QString templateName = QString::fromStdString(item.templateName);

        bool searchMatch =
            searchText.isEmpty() ||
            truckNumber.contains(searchText, Qt::CaseInsensitive) ||
            productName.contains(searchText, Qt::CaseInsensitive) ||
            currentQuantity.contains(searchText, Qt::CaseInsensitive) ||
            minimumQuantity.contains(searchText, Qt::CaseInsensitive) ||
            status.contains(searchText, Qt::CaseInsensitive) ||
            templateName.contains(searchText, Qt::CaseInsensitive);

        bool statusMatch =
            selectedStatus == "All Alerts" ||
            status.compare(selectedStatus, Qt::CaseInsensitive) == 0;

        if (searchMatch && statusMatch) {
            filteredAlerts.push_back(item);
        }
    }

    currentPage = 1;

    populateTable();
    updateMetrics();
    updatePagination();
}

void LowStockAlertsPage::populateTable()
{
    ui.alertsTable->clearContents();

    int totalItems = static_cast<int>(filteredAlerts.size());
    int startIndex = (currentPage - 1) * pageSize;
    int endIndex = (std::min)(startIndex + pageSize, totalItems);
    int rowCount = endIndex - startIndex;

    ui.alertsTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; ++row) {
        const LowStockItemDto& item = filteredAlerts[startIndex + row];

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
}

void LowStockAlertsPage::updateMetrics()
{
    int criticalCount = 0;
    int warningCount = 0;

    for (const LowStockItemDto& item : filteredAlerts) {
        QString status = QString::fromStdString(item.status);

        if (status.compare("Critical", Qt::CaseInsensitive) == 0) {
            criticalCount++;
        }
        else if (status.compare("Warning", Qt::CaseInsensitive) == 0) {
            warningCount++;
        }
    }

    int otherCount =
        static_cast<int>(filteredAlerts.size()) - criticalCount - warningCount;

    ui.metricValue->setText(QString::number(criticalCount));
    ui.metricValue1->setText(QString::number(warningCount));
    ui.metricValue2->setText(QString::number(otherCount));
}

void LowStockAlertsPage::updatePagination()
{
    int totalItems = static_cast<int>(filteredAlerts.size());
    int totalPages = (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage > totalPages) {
        currentPage = totalPages;
    }

    int startItem =
        totalItems == 0 ? 0 : ((currentPage - 1) * pageSize) + 1;

    int endItem =
        (std::min)(currentPage * pageSize, totalItems);

    ui.paginationLabel->setText(
        QString("Showing %1 to %2 of %3 low stock alerts")
        .arg(startItem)
        .arg(endItem)
        .arg(totalItems)
    );

    ui.activePageButton->setText(QString::number(currentPage));
    ui.pageButton2->setText(QString::number(currentPage + 1));

    ui.pageButton->setEnabled(currentPage > 1);
    ui.pageButton3->setEnabled(currentPage < totalPages);
    ui.pageButton2->setVisible(currentPage < totalPages);
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

void LowStockAlertsPage::onPreviousPageClicked()
{
    if (currentPage > 1) {
        currentPage--;
        populateTable();
        updatePagination();
    }
}

void LowStockAlertsPage::onNextPageClicked()
{
    int totalItems = static_cast<int>(filteredAlerts.size());
    int totalPages = (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage < totalPages) {
        currentPage++;
        populateTable();
        updatePagination();
    }
}

void LowStockAlertsPage::onPage2Clicked()
{
    int totalItems = static_cast<int>(filteredAlerts.size());
    int totalPages = (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage + 1 <= totalPages) {
        currentPage++;
        populateTable();
        updatePagination();
    }
}

QString LowStockAlertsPage::escapeCsv(const QString& value) const
{
    QString escaped = value;
    escaped.replace("\"", "\"\"");

    if (escaped.contains(",") || escaped.contains("\"") || escaped.contains("\n")) {
        escaped = "\"" + escaped + "\"";
    }

    return escaped;
}

void LowStockAlertsPage::onExportClicked()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Export Low Stock Alerts",
        "low_stock_alerts.csv",
        "CSV Files (*.csv)"
    );

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(
            this,
            "Export Failed",
            "Could not create the CSV file."
        );
        return;
    }

    QTextStream out(&file);

    out << "Truck,Product,Current Quantity,Minimum Quantity,Status,Template\n";

    for (const LowStockItemDto& item : filteredAlerts) {
        out
            << escapeCsv(QString::fromStdString(item.truckNumber)) << ","
            << escapeCsv(QString::fromStdString(item.productName)) << ","
            << item.currentQuantity << ","
            << item.minimumQuantity << ","
            << escapeCsv(QString::fromStdString(item.status)) << ","
            << escapeCsv(QString::fromStdString(item.templateName)) << "\n";
    }

    file.close();

    QMessageBox::information(
        this,
        "Export Complete",
        "Low stock alerts exported successfully."
    );
}

void LowStockAlertsPage::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::truckPageStyle(theme)
    );
}

