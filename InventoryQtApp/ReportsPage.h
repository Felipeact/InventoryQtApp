#pragma once

#include <QWidget>
#include <QString>
#include <vector>
#include <string>

#include "ui_ReportsPage.h"
#include "ReportService.h"
#include "TruckStockService.h"
#include "Theme.h"

class ReportsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsPage(
        ReportService* reportService,
        TruckStockService* truckStockService,
        QWidget* parent = nullptr
    );

    ~ReportsPage();

    void refreshReports();

    void applyTheme(Theme::AppTheme theme);

private slots:
    void onInventorySummaryClicked();
    void onAssetsSummaryClicked();
    void onTruckSummaryClicked();
    void onTechnicianStockSummaryClicked();
    void onTemplatesSummaryClicked();
    void onAssignmentsSummaryClicked();
    void onReceiptsSummaryClicked();
    void onLowStockSummaryClicked();

    void onExportPdfClicked();
    void onExportExcelClicked();
    void onExportCsvClicked();
    void onRefreshClicked();

private:
    Ui::ReportsPageClass ui;

    ReportService* reportService = nullptr;
    TruckStockService* truckStockService = nullptr;

    QString currentReportName = "Inventory Summary";

    void setupConnections();

    void loadInventorySummary();
    void loadAssetsSummary();
    void loadTruckSummary();
    void loadTechnicianStockSummary();
    void loadTemplatesSummary();
    void loadAssignmentsSummary();
    void loadReceiptsSummary();
    void loadLowStockSummary();

    void displayInventoryReport(const nlohmann::json& data);
    void displayAssetsReport(const nlohmann::json& data);

    void displayGenericReport(
        const QString& title,
        const QStringList& headers,
        const std::vector<QStringList>& rows
    );

    void exportToPdf();
    void exportToExcel();
    void exportToCsv();
};