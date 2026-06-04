#include "ReportsPage.h"
#include "ExportUtility.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QTableWidget>
#include <QTableWidgetItem>

ReportsPage::ReportsPage(
    ReportService* reportService,
    QWidget* parent
)
    : QWidget(parent),
    reportService(reportService)
{
    ui.setupUi(this);
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
    try {
        if (reportService) {
            nlohmann::json data = reportService->getInventorySummary();
            displayInventoryReport(data);
        }
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", "Failed to load inventory summary: " + QString::fromStdString(std::string(e.what())));
    }
}

void ReportsPage::loadAssetsSummary()
{
    try {
        if (reportService) {
            nlohmann::json data = reportService->getAssetsSummary();
            displayAssetsReport(data);
        }
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", "Failed to load assets summary: " + QString::fromStdString(std::string(e.what())));
    }
}

void ReportsPage::displayInventoryReport(const nlohmann::json& data)
{
    ui.reportTable->setRowCount(0);

    try {
        if (data.is_array()) {
            for (size_t i = 0; i < data.size(); ++i) {
                const auto& item = data[i];
                int row = ui.reportTable->rowCount();
                ui.reportTable->insertRow(row);

                // Item name
                QString itemName = QString::fromStdString(
                    item.contains("name") ? item["name"].get<std::string>() : "N/A"
                );
                ui.reportTable->setItem(row, 0, new QTableWidgetItem(itemName));

                // Value
                QString value = QString::fromStdString(
                    item.contains("quantity") ? std::to_string(item["quantity"].get<int>()) : "0"
                );
                ui.reportTable->setItem(row, 1, new QTableWidgetItem(value));

                // Status
                QString status = "Active";
                ui.reportTable->setItem(row, 2, new QTableWidgetItem(status));

                // Details
                QString details = "Inventory Item";
                ui.reportTable->setItem(row, 3, new QTableWidgetItem(details));
            }
        }
    }
    catch (const std::exception& e) {
        QMessageBox::warning(this, "Warning", "Error parsing inventory report: " + QString::fromStdString(std::string(e.what())));
    }
}

void ReportsPage::displayAssetsReport(const nlohmann::json& data)
{
    ui.reportTable->setRowCount(0);

    try {
        if (data.is_array()) {
            for (size_t i = 0; i < data.size(); ++i) {
                const auto& item = data[i];
                int row = ui.reportTable->rowCount();
                ui.reportTable->insertRow(row);

                // Item name
                QString itemName = QString::fromStdString(
                    item.contains("assetName") ? item["assetName"].get<std::string>() : "N/A"
                );
                ui.reportTable->setItem(row, 0, new QTableWidgetItem(itemName));

                // Value
                QString value = QString::fromStdString(
                    item.contains("value") ? std::to_string(item["value"].get<double>()) : "0"
                );
                ui.reportTable->setItem(row, 1, new QTableWidgetItem(value));

                // Status
                QString status = "Active";
                ui.reportTable->setItem(row, 2, new QTableWidgetItem(status));

                // Details
                QString details = "Asset";
                ui.reportTable->setItem(row, 3, new QTableWidgetItem(details));
            }
        }
    }
    catch (const std::exception& e) {
        QMessageBox::warning(this, "Warning", "Error parsing assets report: " + QString::fromStdString(std::string(e.what())));
    }
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

    if (fileName.isEmpty()) return;

    if (ExportUtility::exportTableToPdf(ui.reportTable, fileName)) {
        QString fileSize = ExportUtility::getFileSizeString(fileName);
        QMessageBox::information(this, "Success", 
            "Report exported successfully to:\n" + fileName + 
            "\n\nFile size: " + fileSize);
    } else {
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

    if (fileName.isEmpty()) return;

    if (fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        exportToCsv();
    } else if (fileName.endsWith(".xlsx", Qt::CaseInsensitive)) {
        if (ExportUtility::exportTableToExcel(ui.reportTable, fileName)) {
            QString fileSize = ExportUtility::getFileSizeString(fileName);
            QMessageBox::information(this, "Success",
                "Report exported successfully to:\n" + fileName +
                "\n\nFile size: " + fileSize);
        } else {
            QMessageBox::critical(this, "Error", "Failed to export Excel file.");
        }
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

    if (fileName.isEmpty()) return;

    if (ExportUtility::exportTableToCsv(ui.reportTable, fileName)) {
        QString fileSize = ExportUtility::getFileSizeString(fileName);
        QMessageBox::information(this, "Success",
            "Report exported successfully to:\n" + fileName +
            "\n\nFile size: " + fileSize);
    } else {
        QMessageBox::critical(this, "Error", "Failed to export CSV file.");
    }
}

void ReportsPage::applyTheme(Theme::AppTheme theme)
{
    // Apply theme to the entire page
    this->setStyleSheet(Theme::dataPageStyle(theme));

    // Apply table styling based on theme
    if (theme == Theme::AppTheme::Dark) {
        ui.reportTable->setStyleSheet(
            "QTableWidget { background-color: #2b2b2b; color: #ffffff; gridline-color: #404040; }"
            "QTableWidget::item { padding: 5px; }"
            "QHeaderView::section { background-color: #1e1e1e; color: #ffffff; padding: 5px; border: none; }"
        );
    } else {
        ui.reportTable->setStyleSheet(
            "QTableWidget { background-color: #ffffff; color: #000000; gridline-color: #e0e0e0; }"
            "QTableWidget::item { padding: 5px; }"
            "QHeaderView::section { background-color: #f5f5f5; color: #000000; padding: 5px; border: none; }"
        );
    }

    // Apply button styling
    QString buttonStyle;
    if (theme == Theme::AppTheme::Dark) {
        buttonStyle = 
            "QPushButton { background-color: #0d47a1; color: #ffffff; border-radius: 4px; padding: 6px 12px; }"
            "QPushButton:hover { background-color: #1565c0; }"
            "QPushButton:pressed { background-color: #0d3d8f; }";
    } else {
        buttonStyle = 
            "QPushButton { background-color: #1976d2; color: #ffffff; border-radius: 4px; padding: 6px 12px; }"
            "QPushButton:hover { background-color: #1565c0; }"
            "QPushButton:pressed { background-color: #1565c0; }";
    }

    ui.exportPdfBtn->setStyleSheet(buttonStyle);
    ui.exportExcelBtn->setStyleSheet(buttonStyle);
    ui.exportCsvBtn->setStyleSheet(buttonStyle);
    ui.refreshBtn->setStyleSheet(buttonStyle);
    ui.inventorySummaryBtn->setStyleSheet(buttonStyle);
    ui.assetsSummaryBtn->setStyleSheet(buttonStyle);
}
