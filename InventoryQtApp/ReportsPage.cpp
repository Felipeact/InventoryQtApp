// ReportsPage.cpp

#include "ReportsPage.h"
#include "ExportUtility.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QThread>
#include <QPointer>
#include <QMetaObject>
#include <QPushButton>

namespace
{
    QString jsonValueToString(const nlohmann::json& value)
    {
        try {
            if (value.is_string()) {
                return QString::fromStdString(value.get<std::string>());
            }

            if (value.is_number_integer()) {
                return QString::number(value.get<long long>());
            }

            if (value.is_number_unsigned()) {
                return QString::number(value.get<unsigned long long>());
            }

            if (value.is_number_float()) {
                return QString::number(value.get<double>(), 'f', 2);
            }

            if (value.is_boolean()) {
                return value.get<bool>() ? "Yes" : "No";
            }

            if (value.is_null()) {
                return "N/A";
            }

            return QString::fromStdString(value.dump());
        }
        catch (...) {
            return "N/A";
        }
    }

    QString prettyKey(const std::string& key)
    {
        QString text =
            QString::fromStdString(key);

        text.replace("_", " ");
        text.replace("-", " ");

        QString result;

        for (int i = 0; i < text.length(); ++i) {
            const QChar current =
                text[i];

            if (
                i > 0 &&
                current.isUpper() &&
                text[i - 1].isLower()
                ) {
                result += " ";
            }

            result += current;
        }

        QStringList words =
            result.split(" ", Qt::SkipEmptyParts);

        for (QString& word : words) {
            if (!word.isEmpty()) {
                word[0] = word[0].toUpper();
            }
        }

        return words.join(" ");
    }
}

ReportsPage::ReportsPage(
    ReportService* reportService,
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QWidget(parent),
    reportService(reportService),
    truckStockService(truckStockService)
{
    ui.setupUi(this);

    ui.reportTable->setColumnCount(4);
    ui.reportTable->setHorizontalHeaderLabels({
        "Metric",
        "Value",
        "Status",
        "Details"
        });

    ui.reportTable->horizontalHeader()->setStretchLastSection(true);
    ui.reportTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.reportTable->setAlternatingRowColors(true);
    ui.reportTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.reportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    setupConnections();
    refreshReports();
}

ReportsPage::~ReportsPage()
{
}

void ReportsPage::setupConnections()
{
    connect(ui.inventorySummaryBtn, &QPushButton::clicked, this, &ReportsPage::onInventorySummaryClicked);
    connect(ui.assetsSummaryBtn, &QPushButton::clicked, this, &ReportsPage::onAssetsSummaryClicked);
    connect(ui.truckSummaryBtn, &QPushButton::clicked, this, &ReportsPage::onTruckSummaryClicked);
    connect(ui.templatesSummaryBtn, &QPushButton::clicked, this, &ReportsPage::onTemplatesSummaryClicked);
    connect(ui.assignmentsSummaryBtn, &QPushButton::clicked, this, &ReportsPage::onAssignmentsSummaryClicked);
    connect(ui.receiptsSummaryBtn, &QPushButton::clicked, this, &ReportsPage::onReceiptsSummaryClicked);
    connect(ui.lowStockSummaryBtn, &QPushButton::clicked, this, &ReportsPage::onLowStockSummaryClicked);
    connect(ui.exportPdfBtn, &QPushButton::clicked, this, &ReportsPage::onExportPdfClicked);
    connect(ui.exportExcelBtn, &QPushButton::clicked, this, &ReportsPage::onExportExcelClicked);
    connect(ui.exportCsvBtn, &QPushButton::clicked, this, &ReportsPage::onExportCsvClicked);
    connect(ui.refreshBtn, &QPushButton::clicked, this, &ReportsPage::onRefreshClicked);
}

void ReportsPage::refreshReports()
{
    if (currentReportName == "Assets Summary") {
        loadAssetsSummary();
    }
    else if (currentReportName == "Truck Summary") {
        loadTruckSummary();
    }
    else if (currentReportName == "Templates Summary") {
        loadTemplatesSummary();
    }
    else if (currentReportName == "Assignments Summary") {
        loadAssignmentsSummary();
    }
    else if (currentReportName == "Receipts Summary") {
        loadReceiptsSummary();
    }
    else if (currentReportName == "Low Stock Summary") {
        loadLowStockSummary();
    }
    else {
        loadInventorySummary();
    }
}

void ReportsPage::onInventorySummaryClicked()
{
    loadInventorySummary();
}

void ReportsPage::onAssetsSummaryClicked()
{
    loadAssetsSummary();
}

void ReportsPage::onTruckSummaryClicked()
{
    loadTruckSummary();
}

void ReportsPage::onTemplatesSummaryClicked()
{
    loadTemplatesSummary();
}

void ReportsPage::onAssignmentsSummaryClicked()
{
    loadAssignmentsSummary();
}

void ReportsPage::onReceiptsSummaryClicked()
{
    loadReceiptsSummary();
}

void ReportsPage::onLowStockSummaryClicked()
{
    loadLowStockSummary();
}

void ReportsPage::loadInventorySummary()
{
    currentReportName = "Inventory Summary";

    if (!reportService) {
        QMessageBox::warning(this, "Reports", "Report service is not available.");
        return;
    }

    QPointer<ReportsPage> self(this);
    ReportService* service = reportService;

    QThread* worker =
        QThread::create([self, service]() {
        try {
            nlohmann::json data =
                service->getInventorySummary();

            if (!self) {
                return;
            }

            QMetaObject::invokeMethod(
                self,
                [self, data]() {
                    if (!self) {
                        return;
                    }

                    self->displayInventoryReport(data);
                },
                Qt::QueuedConnection
            );
        }
        catch (const std::exception& e) {
            QString message =
                QString::fromStdString(e.what());

            if (!self) {
                return;
            }

            QMetaObject::invokeMethod(
                self,
                [self, message]() {
                    if (!self) {
                        return;
                    }

                    QMessageBox::critical(
                        self,
                        "Error",
                        "Failed to load inventory summary: " + message
                    );
                },
                Qt::QueuedConnection
            );
        }
            });

    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void ReportsPage::loadAssetsSummary()
{
    currentReportName = "Assets Summary";

    if (!reportService) {
        QMessageBox::warning(this, "Reports", "Report service is not available.");
        return;
    }

    QPointer<ReportsPage> self(this);
    ReportService* service = reportService;

    QThread* worker =
        QThread::create([self, service]() {
        try {
            nlohmann::json data =
                service->getAssetsSummary();

            if (!self) {
                return;
            }

            QMetaObject::invokeMethod(
                self,
                [self, data]() {
                    if (!self) {
                        return;
                    }

                    self->displayAssetsReport(data);
                },
                Qt::QueuedConnection
            );
        }
        catch (const std::exception& e) {
            QString message =
                QString::fromStdString(e.what());

            if (!self) {
                return;
            }

            QMetaObject::invokeMethod(
                self,
                [self, message]() {
                    if (!self) {
                        return;
                    }

                    QMessageBox::critical(
                        self,
                        "Error",
                        "Failed to load assets summary: " + message
                    );
                },
                Qt::QueuedConnection
            );
        }
            });

    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void ReportsPage::loadTruckSummary()
{
    currentReportName = "Truck Summary";

    if (!truckStockService) {
        displayGenericReport(
            "Truck Summary",
            QStringList() << "Truck" << "Plate" << "Technician" << "Status" << "Stock Status",
            {
                QStringList() << "No service available" << "N/A" << "N/A" << "Error" << "Truck stock service unavailable"
            }
        );
        return;
    }

    std::vector<TruckDto> trucks =
        truckStockService->getTrucks();

    std::vector<QStringList> rows;

    if (trucks.empty()) {
        rows.push_back(
            QStringList()
            << "No trucks found"
            << ""
            << ""
            << "Empty"
            << ""
        );
    }
    else {
        for (const TruckDto& truck : trucks) {
            rows.push_back(
                QStringList()
                << QString::fromStdString(truck.truckName)
                << QString::fromStdString(truck.licensePlate)
                << QString::fromStdString(truck.technicianName)
                << QString::fromStdString(truck.status)
                << "Normal"
            );
        }
    }

    displayGenericReport(
        "Truck Summary",
        QStringList() << "Truck" << "Plate" << "Technician" << "Status" << "Stock Status",
        rows
    );
}

void ReportsPage::loadTemplatesSummary()
{
    currentReportName = "Templates Summary";

    if (!truckStockService) {
        displayGenericReport(
            "Templates Summary",
            QStringList() << "Template" << "Trade Type" << "Items" << "Status",
            {
                QStringList() << "No service available" << "N/A" << "N/A" << "Error"
            }
        );
        return;
    }

    std::vector<StockTemplateDto> templates =
        truckStockService->getTemplates();

    std::vector<QStringList> rows;

    if (templates.empty()) {
        rows.push_back(
            QStringList()
            << "No templates found"
            << ""
            << "0"
            << "Empty"
        );
    }
    else {
        for (const StockTemplateDto& stockTemplate : templates) {
            rows.push_back(
                QStringList()
                << QString::fromStdString(stockTemplate.name)
                << QString::fromStdString(stockTemplate.tradeType)
                << QString::number(stockTemplate.itemCount)
                << "Available"
            );
        }
    }

    displayGenericReport(
        "Templates Summary",
        QStringList() << "Template" << "Trade Type" << "Items" << "Status",
        rows
    );
}

void ReportsPage::loadAssignmentsSummary()
{
    currentReportName = "Assignments Summary";

    if (!truckStockService) {
        displayGenericReport(
            "Assignments Summary",
            QStringList() << "Truck" << "Template" << "Assigned By" << "Assigned On" << "Status",
            {
                QStringList() << "No service available" << "N/A" << "N/A" << "N/A" << "Error"
            }
        );
        return;
    }

    std::vector<TruckAssignmentDto> assignments =
        truckStockService->getAssignments();

    std::vector<QStringList> rows;

    if (assignments.empty()) {
        rows.push_back(
            QStringList()
            << "No assignments found"
            << ""
            << ""
            << ""
            << "Empty"
        );
    }
    else {
        for (const TruckAssignmentDto& assignment : assignments) {
            rows.push_back(
                QStringList()
                << QString::fromStdString(assignment.truckNumber)
                << QString::fromStdString(assignment.templateName)
                << QString::fromStdString(assignment.assignedBy)
                << QString::fromStdString(assignment.assignedOn)
                << QString::fromStdString(assignment.status)
            );
        }
    }

    displayGenericReport(
        "Assignments Summary",
        QStringList() << "Truck" << "Template" << "Assigned By" << "Assigned On" << "Status",
        rows
    );
}

void ReportsPage::loadReceiptsSummary()
{
    currentReportName = "Receipts Summary";

    if (!truckStockService) {
        displayGenericReport(
            "Receipts Summary",
            QStringList() << "Receipt" << "Technician" << "Truck" << "Amount" << "Status" << "Date",
            {
                QStringList() << "No service available" << "N/A" << "N/A" << "N/A" << "Error" << "N/A"
            }
        );
        return;
    }

    std::vector<ReceiptDto> receipts =
        truckStockService->getReceipts();

    std::vector<QStringList> rows;

    if (receipts.empty()) {
        rows.push_back(
            QStringList()
            << "No receipts found"
            << ""
            << ""
            << "$0.00"
            << "Empty"
            << ""
        );
    }
    else {
        for (const ReceiptDto& receipt : receipts) {
            rows.push_back(
                QStringList()
                << QString::fromStdString(receipt.id)
                << QString::fromStdString(receipt.technicianName)
                << QString::fromStdString(receipt.truckNumber)
                << "$" + QString::number(receipt.totalAmount, 'f', 2)
                << QString::fromStdString(receipt.status)
                << QString::fromStdString(receipt.createdAt)
            );
        }
    }

    displayGenericReport(
        "Receipts Summary",
        QStringList() << "Receipt" << "Technician" << "Truck" << "Amount" << "Status" << "Date",
        rows
    );
}

void ReportsPage::loadLowStockSummary()
{
    currentReportName = "Low Stock Summary";

    if (!truckStockService) {
        displayGenericReport(
            "Low Stock Summary",
            QStringList() << "Truck" << "Item" << "Current" << "Minimum" << "Status" << "Template",
            {
                QStringList() << "No service available" << "N/A" << "N/A" << "N/A" << "Error" << "N/A"
            }
        );
        return;
    }

    std::vector<LowStockItemDto> lowStockItems =
        truckStockService->getLowStockItems();

    std::vector<QStringList> rows;

    if (lowStockItems.empty()) {
        rows.push_back(
            QStringList()
            << "No low stock items found"
            << ""
            << "0"
            << "0"
            << "Empty"
            << ""
        );
    }
    else {
        for (const LowStockItemDto& item : lowStockItems) {
            rows.push_back(
                QStringList()
                << QString::fromStdString(item.truckNumber)
                << QString::fromStdString(item.productName)
                << QString::number(item.currentQuantity)
                << QString::number(item.minimumQuantity)
                << QString::fromStdString(item.status)
                << QString::fromStdString(item.templateName)
            );
        }
    }

    displayGenericReport(
        "Low Stock Summary",
        QStringList() << "Truck" << "Item" << "Current" << "Minimum" << "Status" << "Template",
        rows
    );
}

void ReportsPage::displayInventoryReport(
    const nlohmann::json& data
)
{
    currentReportName = "Inventory Summary";

    ui.reportTable->setUpdatesEnabled(false);
    ui.reportTable->setRowCount(0);
    ui.reportTable->setColumnCount(4);

    ui.reportTable->setHorizontalHeaderLabels({
        "Inventory Metric",
        "Value",
        "Status",
        "Details"
        });

    try {
        nlohmann::json reportData =
            data;

        if (data.contains("data")) {
            reportData = data["data"];
        }
        else if (data.contains("summary")) {
            reportData = data["summary"];
        }
        else if (data.contains("inventorySummary")) {
            reportData = data["inventorySummary"];
        }

        auto addRow =
            [this](
                const QString& metric,
                const QString& value,
                const QString& status,
                const QString& details
                ) {
                    int row =
                        ui.reportTable->rowCount();

                    ui.reportTable->insertRow(row);

                    ui.reportTable->setItem(row, 0, new QTableWidgetItem(metric));
                    ui.reportTable->setItem(row, 1, new QTableWidgetItem(value));
                    ui.reportTable->setItem(row, 2, new QTableWidgetItem(status));
                    ui.reportTable->setItem(row, 3, new QTableWidgetItem(details));
            };

        if (reportData.is_object()) {
            for (auto it = reportData.begin(); it != reportData.end(); ++it) {
                addRow(
                    prettyKey(it.key()),
                    jsonValueToString(it.value()),
                    "Summary",
                    "Inventory Summary"
                );
            }
        }
        else if (reportData.is_array()) {
            for (const auto& item : reportData) {
                QString metric = "Inventory Item";
                QString value = "0";
                QString status = "Active";
                QString details = "Inventory Detail";

                if (item.is_object()) {
                    if (item.contains("name")) {
                        metric = jsonValueToString(item["name"]);
                    }
                    else if (item.contains("productName")) {
                        metric = jsonValueToString(item["productName"]);
                    }
                    else if (item.contains("itemName")) {
                        metric = jsonValueToString(item["itemName"]);
                    }
                    else if (item.contains("label")) {
                        metric = jsonValueToString(item["label"]);
                    }

                    if (item.contains("quantity")) {
                        value = jsonValueToString(item["quantity"]);
                    }
                    else if (item.contains("value")) {
                        value = jsonValueToString(item["value"]);
                    }
                    else if (item.contains("total")) {
                        value = jsonValueToString(item["total"]);
                    }

                    if (item.contains("status")) {
                        status = jsonValueToString(item["status"]);
                    }

                    if (item.contains("details")) {
                        details = jsonValueToString(item["details"]);
                    }
                }

                addRow(metric, value, status, details);
            }
        }
        else {
            addRow(
                "Inventory Summary",
                jsonValueToString(reportData),
                "Summary",
                "Inventory Summary"
            );
        }

        if (ui.reportTable->rowCount() == 0) {
            addRow(
                "No Data",
                "0",
                "Empty",
                "No inventory report data returned."
            );
        }
    }
    catch (const std::exception& e) {
        QMessageBox::warning(
            this,
            "Warning",
            "Error parsing inventory report: " +
            QString::fromStdString(e.what())
        );
    }

    ui.reportTable->setUpdatesEnabled(true);
}

void ReportsPage::displayAssetsReport(
    const nlohmann::json& data
)
{
    currentReportName = "Assets Summary";

    ui.reportTable->setUpdatesEnabled(false);
    ui.reportTable->setRowCount(0);
    ui.reportTable->setColumnCount(4);

    ui.reportTable->setHorizontalHeaderLabels({
        "Asset Metric",
        "Value",
        "Status",
        "Details"
        });

    try {
        nlohmann::json reportData =
            data;

        if (data.contains("data")) {
            reportData = data["data"];
        }
        else if (data.contains("summary")) {
            reportData = data["summary"];
        }
        else if (data.contains("assetsSummary")) {
            reportData = data["assetsSummary"];
        }
        else if (data.contains("assetSummary")) {
            reportData = data["assetSummary"];
        }

        auto addRow =
            [this](
                const QString& metric,
                const QString& value,
                const QString& status,
                const QString& details
                ) {
                    int row =
                        ui.reportTable->rowCount();

                    ui.reportTable->insertRow(row);

                    ui.reportTable->setItem(row, 0, new QTableWidgetItem(metric));
                    ui.reportTable->setItem(row, 1, new QTableWidgetItem(value));
                    ui.reportTable->setItem(row, 2, new QTableWidgetItem(status));
                    ui.reportTable->setItem(row, 3, new QTableWidgetItem(details));
            };

        if (reportData.is_object()) {
            for (auto it = reportData.begin(); it != reportData.end(); ++it) {
                addRow(
                    prettyKey(it.key()),
                    jsonValueToString(it.value()),
                    "Summary",
                    "Asset Summary"
                );
            }
        }
        else if (reportData.is_array()) {
            for (const auto& item : reportData) {
                QString metric = "Asset";
                QString value = "0";
                QString status = "Active";
                QString details = "Asset Detail";

                if (item.is_object()) {
                    if (item.contains("assetName")) {
                        metric = jsonValueToString(item["assetName"]);
                    }
                    else if (item.contains("name")) {
                        metric = jsonValueToString(item["name"]);
                    }
                    else if (item.contains("label")) {
                        metric = jsonValueToString(item["label"]);
                    }

                    if (item.contains("value")) {
                        value = jsonValueToString(item["value"]);
                    }
                    else if (item.contains("quantity")) {
                        value = jsonValueToString(item["quantity"]);
                    }
                    else if (item.contains("total")) {
                        value = jsonValueToString(item["total"]);
                    }

                    if (item.contains("status")) {
                        status = jsonValueToString(item["status"]);
                    }

                    if (item.contains("details")) {
                        details = jsonValueToString(item["details"]);
                    }
                }

                addRow(metric, value, status, details);
            }
        }
        else {
            addRow(
                "Asset Summary",
                jsonValueToString(reportData),
                "Summary",
                "Asset Summary"
            );
        }

        if (ui.reportTable->rowCount() == 0) {
            addRow(
                "No Data",
                "0",
                "Empty",
                "No asset report data returned."
            );
        }
    }
    catch (const std::exception& e) {
        QMessageBox::warning(
            this,
            "Warning",
            "Error parsing assets report: " +
            QString::fromStdString(e.what())
        );
    }

    ui.reportTable->setUpdatesEnabled(true);
}

void ReportsPage::displayGenericReport(
    const QString& title,
    const QStringList& headers,
    const std::vector<QStringList>& rows
)
{
    currentReportName = title;

    ui.reportTable->setUpdatesEnabled(false);
    ui.reportTable->clearContents();
    ui.reportTable->clearSpans();
    ui.reportTable->setRowCount(0);
    ui.reportTable->setColumnCount(headers.size());
    ui.reportTable->setHorizontalHeaderLabels(headers);

    for (const QStringList& rowData : rows) {
        int row =
            ui.reportTable->rowCount();

        ui.reportTable->insertRow(row);

        for (int col = 0; col < headers.size(); col++) {
            QString value;

            if (col < rowData.size()) {
                value = rowData[col];
            }

            ui.reportTable->setItem(
                row,
                col,
                new QTableWidgetItem(value)
            );
        }
    }

    ui.reportTable->setUpdatesEnabled(true);
}

void ReportsPage::onExportPdfClicked()
{
    exportToPdf();
}

void ReportsPage::onExportExcelClicked()
{
    exportToExcel();
}

void ReportsPage::onExportCsvClicked()
{
    exportToCsv();
}

void ReportsPage::onRefreshClicked()
{
    refreshReports();
}

void ReportsPage::exportToPdf()
{
    QString defaultFile =
        currentReportName.replace(" ", "_") + ".pdf";

    QString fileName =
        QFileDialog::getSaveFileName(
            this,
            "Export Report to PDF",
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + defaultFile,
            "PDF Files (*.pdf)"
        );

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
        fileName += ".pdf";
    }

    if (ExportUtility::exportTableToPdf(ui.reportTable, fileName)) {
        QMessageBox::information(
            this,
            "Success",
            "Report exported successfully to:\n" +
            fileName +
            "\n\nFile size: " +
            ExportUtility::getFileSizeString(fileName)
        );
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to export PDF file.");
    }
}

void ReportsPage::exportToExcel()
{
    QString defaultFile =
        currentReportName.replace(" ", "_") + ".xlsx";

    QString fileName =
        QFileDialog::getSaveFileName(
            this,
            "Export Report to Excel",
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + defaultFile,
            "Excel Files (*.xlsx);;CSV Files (*.csv)"
        );

    if (fileName.isEmpty()) {
        return;
    }

    if (
        !fileName.endsWith(".xlsx", Qt::CaseInsensitive) &&
        !fileName.endsWith(".csv", Qt::CaseInsensitive)
        ) {
        fileName += ".xlsx";
    }

    if (fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        if (ExportUtility::exportTableToCsv(ui.reportTable, fileName)) {
            QMessageBox::information(
                this,
                "Success",
                "Report exported successfully to:\n" +
                fileName +
                "\n\nFile size: " +
                ExportUtility::getFileSizeString(fileName)
            );
        }
        else {
            QMessageBox::critical(this, "Error", "Failed to export CSV file.");
        }

        return;
    }

    if (ExportUtility::exportTableToExcel(ui.reportTable, fileName)) {
        QMessageBox::information(
            this,
            "Success",
            "Report exported successfully to:\n" +
            fileName +
            "\n\nFile size: " +
            ExportUtility::getFileSizeString(fileName)
        );
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to export Excel file.");
    }
}

void ReportsPage::exportToCsv()
{
    QString defaultFile =
        currentReportName.replace(" ", "_") + ".csv";

    QString fileName =
        QFileDialog::getSaveFileName(
            this,
            "Export Report to CSV",
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + defaultFile,
            "CSV Files (*.csv)"
        );

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }

    if (ExportUtility::exportTableToCsv(ui.reportTable, fileName)) {
        QMessageBox::information(
            this,
            "Success",
            "Report exported successfully to:\n" +
            fileName +
            "\n\nFile size: " +
            ExportUtility::getFileSizeString(fileName)
        );
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to export CSV file.");
    }
}

void ReportsPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::reportsPageStyle(theme)
    );
}