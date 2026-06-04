#pragma once

#include <QWidget>
#include <QString>
#include <vector>
#include <string>

#include "ui_ReportsPage.h"
#include "ReportService.h"
#include "Theme.h"

class ReportsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsPage(
        ReportService* reportService,
        QWidget* parent = nullptr
    );

    ~ReportsPage();

    void refreshReports();

    void applyTheme(Theme::AppTheme theme);

private slots:
    void onInventorySummaryClicked();
    void onAssetsSummaryClicked();
    void onExportPdfClicked();
    void onExportExcelClicked();
    void onExportCsvClicked();
    void onRefreshClicked();

private:
    Ui::ReportsPageClass ui;

    ReportService* reportService = nullptr;

    void setupConnections();
    void loadInventorySummary();
    void loadAssetsSummary();
    void displayInventoryReport(const nlohmann::json& data);
    void displayAssetsReport(const nlohmann::json& data);
    void exportToPdf();
    void exportToExcel();
    void exportToCsv();
};
