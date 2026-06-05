#include "LowStockAlertsPage.h"
#include "Theme.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFrame>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextStream>

LowStockAlertsPage::LowStockAlertsPage(
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);

    applyTheme(Theme::AppTheme::Dark);

    setupTable();
    setupConnections();
    loadAlerts();
}

LowStockAlertsPage::~LowStockAlertsPage()
{
}

void LowStockAlertsPage::setupConnections()
{
    connect(
        ui.searchInput,
        &QLineEdit::textChanged,
        this,
        &LowStockAlertsPage::onSearchChanged
    );

    connect(
        ui.statusFilter,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &LowStockAlertsPage::onStatusFilterChanged
    );

    connect(
        ui.exportButton,
        &QPushButton::clicked,
        this,
        &LowStockAlertsPage::onExportClicked
    );

    connect(
        ui.pageButton,
        &QPushButton::clicked,
        this,
        &LowStockAlertsPage::onPreviousPageClicked
    );

    connect(
        ui.pageButton3,
        &QPushButton::clicked,
        this,
        &LowStockAlertsPage::onNextPageClicked
    );

    connect(
        ui.pageButton2,
        &QPushButton::clicked,
        this,
        &LowStockAlertsPage::onPage2Clicked
    );
}

void LowStockAlertsPage::setupTable()
{
    ui.alertsTable->setColumnCount(6);

    ui.alertsTable->setHorizontalHeaderLabels(
        QStringList()
        << "Truck"
        << "Item"
        << "Current Qty"
        << "Minimum Qty"
        << "Status"
        << "Template"
    );

    ui.alertsTable->verticalHeader()->setVisible(false);
    ui.alertsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.alertsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.alertsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.alertsTable->setShowGrid(false);
    ui.alertsTable->setFrameShape(QFrame::NoFrame);
    ui.alertsTable->setFocusPolicy(Qt::NoFocus);
    ui.alertsTable->setAlternatingRowColors(false);
    ui.alertsTable->viewport()->setAutoFillBackground(false);

    ui.alertsTable->horizontalHeader()->setHighlightSections(false);
    ui.alertsTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    ui.alertsTable->horizontalHeader()->setFixedHeight(48);
    ui.alertsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui.alertsTable->verticalHeader()->setDefaultSectionSize(52);

    ui.alertsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.alertsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void LowStockAlertsPage::loadAlerts()
{
    if (!truckStockService) {
        currentAlerts.clear();
        filteredAlerts.clear();

        populateTable();
        updateMetrics();
        updatePagination();

        return;
    }

    currentAlerts =
        truckStockService->getLowStockItems();

    filterAlerts();
}

void LowStockAlertsPage::refreshAlerts()
{
    loadAlerts();
}

void LowStockAlertsPage::filterAlerts()
{
    QString searchText =
        ui.searchInput->text().trimmed();

    QString selectedStatus =
        ui.statusFilter->currentText();

    filteredAlerts.clear();

    for (const LowStockItemDto& item : currentAlerts) {
        QString truckNumber =
            QString::fromStdString(item.truckNumber);

        QString productName =
            QString::fromStdString(item.productName);

        QString currentQuantity =
            QString::number(item.currentQuantity);

        QString minimumQuantity =
            QString::number(item.minimumQuantity);

        QString status =
            QString::fromStdString(item.status);

        QString templateName =
            QString::fromStdString(item.templateName);

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
    ui.alertsTable->clearSpans();

    int totalItems =
        static_cast<int>(filteredAlerts.size());

    int startIndex =
        (currentPage - 1) * pageSize;

    int endIndex =
        (std::min)(startIndex + pageSize, totalItems);

    int rowCount =
        endIndex - startIndex;

    if (rowCount == 0) {
        ui.alertsTable->setRowCount(1);

        ui.alertsTable->setItem(
            0,
            0,
            new QTableWidgetItem("No low stock alerts found")
        );

        ui.alertsTable->setSpan(0, 0, 1, 6);

        return;
    }

    ui.alertsTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const LowStockItemDto& item =
            filteredAlerts[startIndex + row];

        ui.alertsTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::fromStdString(item.truckNumber))
        );

        ui.alertsTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(item.productName))
        );

        ui.alertsTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::number(item.currentQuantity))
        );

        ui.alertsTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::number(item.minimumQuantity))
        );

        ui.alertsTable->setItem(
            row,
            4,
            new QTableWidgetItem(QString::fromStdString(item.status))
        );

        ui.alertsTable->setItem(
            row,
            5,
            new QTableWidgetItem(QString::fromStdString(item.templateName))
        );
    }
}

void LowStockAlertsPage::updateMetrics()
{
    int criticalCount = 0;
    int warningCount = 0;

    for (const LowStockItemDto& item : filteredAlerts) {
        QString status =
            QString::fromStdString(item.status);

        if (status.compare("Critical", Qt::CaseInsensitive) == 0) {
            criticalCount++;
        }
        else if (status.compare("Warning", Qt::CaseInsensitive) == 0) {
            warningCount++;
        }
    }

    int healthyCount =
        static_cast<int>(filteredAlerts.size()) -
        criticalCount -
        warningCount;

    ui.metricValue->setText(QString::number(criticalCount));
    ui.metricValue1->setText(QString::number(warningCount));
    ui.metricValue2->setText(QString::number(healthyCount));
}

void LowStockAlertsPage::updatePagination()
{
    int totalItems =
        static_cast<int>(filteredAlerts.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage > totalPages) {
        currentPage = totalPages;
    }

    int startItem =
        totalItems == 0
        ? 0
        : ((currentPage - 1) * pageSize) + 1;

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

void LowStockAlertsPage::onSearchChanged(
    const QString& text
)
{
    Q_UNUSED(text);

    filterAlerts();
}

void LowStockAlertsPage::onStatusFilterChanged(
    int index
)
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
    int totalItems =
        static_cast<int>(filteredAlerts.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage < totalPages) {
        currentPage++;

        populateTable();
        updatePagination();
    }
}

void LowStockAlertsPage::onPage2Clicked()
{
    int totalItems =
        static_cast<int>(filteredAlerts.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage + 1 <= totalPages) {
        currentPage++;

        populateTable();
        updatePagination();
    }
}

QString LowStockAlertsPage::escapeCsv(
    const QString& value
) const
{
    QString escaped = value;

    escaped.replace("\"", "\"\"");

    if (
        escaped.contains(",") ||
        escaped.contains("\"") ||
        escaped.contains("\n")
        ) {
        escaped = "\"" + escaped + "\"";
    }

    return escaped;
}

void LowStockAlertsPage::onExportClicked()
{
    QString filePath =
        QFileDialog::getSaveFileName(
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

void LowStockAlertsPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::lowStockAlertsPageStyle(theme)
    );
}

void LowStockAlertsPage::setSearchText(const QString& text)
{
	ui.searchInput->setText(text);
}
