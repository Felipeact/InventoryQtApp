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
        QString text = QString::fromStdString(key);

        text.replace("_", " ");
        text.replace("-", " ");

        QString result;

        for (int i = 0; i < text.length(); ++i) {
            const QChar current = text[i];

            if (i > 0 && current.isUpper() && text[i - 1].isLower()) {
                result += " ";
            }

            result += current;
        }

        QStringList words = result.split(" ", Qt::SkipEmptyParts);

        for (QString& word : words) {
            if (!word.isEmpty()) {
                word[0] = word[0].toUpper();
            }
        }

        return words.join(" ");
    }
}

ReportsPage::ReportsPage(ReportService* reportService, QWidget* parent)
    : QWidget(parent),
    reportService(reportService)
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
    connect(ui.exportPdfBtn, &QPushButton::clicked, this, &ReportsPage::onExportPdfClicked);
    connect(ui.exportExcelBtn, &QPushButton::clicked, this, &ReportsPage::onExportExcelClicked);
    connect(ui.exportCsvBtn, &QPushButton::clicked, this, &ReportsPage::onExportCsvClicked);
    connect(ui.refreshBtn, &QPushButton::clicked, this, &ReportsPage::onRefreshClicked);
}

void ReportsPage::refreshReports()
{
    loadInventorySummary();
}

void ReportsPage::onInventorySummaryClicked()
{
    loadInventorySummary();
}

void ReportsPage::onAssetsSummaryClicked()
{
    loadAssetsSummary();
}

void ReportsPage::loadInventorySummary()
{
    if (!reportService) {
        QMessageBox::warning(this, "Reports", "Report service is not available.");
        return;
    }

    QPointer<ReportsPage> self(this);
    ReportService* service = reportService;
    QThread* worker = QThread::create([self, service]() {
        try {
            nlohmann::json data = service->getInventorySummary();
            if (!self) return;
            QMetaObject::invokeMethod(self, [self, data]() {
                if (!self) return;
                self->displayInventoryReport(data);
                }, Qt::QueuedConnection);
        }
        catch (const std::exception& e) {
            QString message = QString::fromStdString(e.what());
            if (!self) return;
            QMetaObject::invokeMethod(self, [self, message]() {
                if (!self) return;
                QMessageBox::critical(self, "Error", "Failed to load inventory summary: " + message);
                }, Qt::QueuedConnection);
        }
        });
    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void ReportsPage::loadAssetsSummary()
{
    if (!reportService) {
        QMessageBox::warning(this, "Reports", "Report service is not available.");
        return;
    }

    QPointer<ReportsPage> self(this);
    ReportService* service = reportService;
    QThread* worker = QThread::create([self, service]() {
        try {
            nlohmann::json data = service->getAssetsSummary();
            if (!self) return;
            QMetaObject::invokeMethod(self, [self, data]() {
                if (!self) return;
                self->displayAssetsReport(data);
                }, Qt::QueuedConnection);
        }
        catch (const std::exception& e) {
            QString message = QString::fromStdString(e.what());
            if (!self) return;
            QMetaObject::invokeMethod(self, [self, message]() {
                if (!self) return;
                QMessageBox::critical(self, "Error", "Failed to load assets summary: " + message);
                }, Qt::QueuedConnection);
        }
        });
    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void ReportsPage::displayInventoryReport(const nlohmann::json& data)
{
    ui.reportTable->setUpdatesEnabled(false);
    ui.reportTable->setRowCount(0);
    ui.reportTable->setHorizontalHeaderLabels({
        "Inventory Metric",
        "Value",
        "Status",
        "Details"
        });

    try {
        nlohmann::json reportData = data;

        if (data.contains("data")) {
            reportData = data["data"];
        }
        else if (data.contains("summary")) {
            reportData = data["summary"];
        }
        else if (data.contains("inventorySummary")) {
            reportData = data["inventorySummary"];
        }

        auto addRow = [this](const QString& metric, const QString& value, const QString& status, const QString& details) {
            int row = ui.reportTable->rowCount();
            ui.reportTable->insertRow(row);

            ui.reportTable->setItem(row, 0, new QTableWidgetItem(metric));
            ui.reportTable->setItem(row, 1, new QTableWidgetItem(value));
            ui.reportTable->setItem(row, 2, new QTableWidgetItem(status));
            ui.reportTable->setItem(row, 3, new QTableWidgetItem(details));
            };

        if (reportData.is_object()) {
            for (auto it = reportData.begin(); it != reportData.end(); ++it) {
                QString metric = prettyKey(it.key());
                QString value = jsonValueToString(it.value());
                QString status = "Summary";
                QString details = "Inventory Summary";

                addRow(metric, value, status, details);
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
            addRow("Inventory Summary", jsonValueToString(reportData), "Summary", "Inventory Summary");
        }

        if (ui.reportTable->rowCount() == 0) {
            addRow("No Data", "0", "Empty", "No inventory report data returned.");
        }
    }
    catch (const std::exception& e) {
        QMessageBox::warning(
            this,
            "Warning",
            "Error parsing inventory report: " + QString::fromStdString(e.what())
        );
    }

    ui.reportTable->setUpdatesEnabled(true);
}

void ReportsPage::displayAssetsReport(const nlohmann::json& data)
{
    ui.reportTable->setUpdatesEnabled(false);
    ui.reportTable->setRowCount(0);
    ui.reportTable->setHorizontalHeaderLabels({
        "Asset Metric",
        "Value",
        "Status",
        "Details"
        });

    try {
        nlohmann::json reportData = data;

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

        auto addRow = [this](const QString& metric, const QString& value, const QString& status, const QString& details) {
            int row = ui.reportTable->rowCount();
            ui.reportTable->insertRow(row);

            ui.reportTable->setItem(row, 0, new QTableWidgetItem(metric));
            ui.reportTable->setItem(row, 1, new QTableWidgetItem(value));
            ui.reportTable->setItem(row, 2, new QTableWidgetItem(status));
            ui.reportTable->setItem(row, 3, new QTableWidgetItem(details));
            };

        if (reportData.is_object()) {
            for (auto it = reportData.begin(); it != reportData.end(); ++it) {
                QString metric = prettyKey(it.key());
                QString value = jsonValueToString(it.value());
                QString status = "Summary";
                QString details = "Asset Summary";

                addRow(metric, value, status, details);
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
            addRow("Asset Summary", jsonValueToString(reportData), "Summary", "Asset Summary");
        }

        if (ui.reportTable->rowCount() == 0) {
            addRow("No Data", "0", "Empty", "No asset report data returned.");
        }
    }
    catch (const std::exception& e) {
        QMessageBox::warning(
            this,
            "Warning",
            "Error parsing assets report: " + QString::fromStdString(e.what())
        );
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
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Report to PDF",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "PDF Files (*.pdf)"
    );

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
        fileName += ".pdf";
    }

    if (ExportUtility::exportTableToPdf(ui.reportTable, fileName)) {
        QString fileSize = ExportUtility::getFileSizeString(fileName);

        QMessageBox::information(
            this,
            "Success",
            "Report exported successfully to:\n" + fileName +
            "\n\nFile size: " + fileSize
        );
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to export PDF file.");
    }
}

void ReportsPage::exportToExcel()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Report to Excel",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Excel Files (*.xlsx);;CSV Files (*.csv)"
    );

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".xlsx", Qt::CaseInsensitive) &&
        !fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".xlsx";
    }

    if (fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        if (ExportUtility::exportTableToCsv(ui.reportTable, fileName)) {
            QString fileSize = ExportUtility::getFileSizeString(fileName);

            QMessageBox::information(
                this,
                "Success",
                "Report exported successfully to:\n" + fileName +
                "\n\nFile size: " + fileSize
            );
        }
        else {
            QMessageBox::critical(this, "Error", "Failed to export CSV file.");
        }

        return;
    }

    if (ExportUtility::exportTableToExcel(ui.reportTable, fileName)) {
        QString fileSize = ExportUtility::getFileSizeString(fileName);

        QMessageBox::information(
            this,
            "Success",
            "Report exported successfully to:\n" + fileName +
            "\n\nFile size: " + fileSize
        );
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to export Excel file.");
    }
}

void ReportsPage::exportToCsv()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Report to CSV",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "CSV Files (*.csv)"
    );

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }

    if (ExportUtility::exportTableToCsv(ui.reportTable, fileName)) {
        QString fileSize = ExportUtility::getFileSizeString(fileName);

        QMessageBox::information(
            this,
            "Success",
            "Report exported successfully to:\n" + fileName +
            "\n\nFile size: " + fileSize
        );
    }
    else {
        QMessageBox::critical(this, "Error", "Failed to export CSV file.");
    }
}

void ReportsPage::applyTheme(Theme::AppTheme theme)
{
    this->setStyleSheet(Theme::reportsPageStyle(theme));
}