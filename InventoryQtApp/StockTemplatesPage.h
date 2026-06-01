#pragma once

#include <QWidget>
#include <QString>
#include <vector>

#include "ui_StockTemplatesPage.h"
#include "TruckStockService.h"
#include "Theme.h"

class StockTemplatesPage : public QWidget
{
    Q_OBJECT

public:

    void applyTheme(Theme::AppTheme theme);
    StockTemplatesPage(TruckStockService* truckStockService, QWidget* parent = nullptr);
    ~StockTemplatesPage();

    void refreshTemplatesList();

private slots:
    void onNewTemplateClicked();
    void onSearchChanged(const QString& text);

    void onPreviousPageClicked();
    void onNextPageClicked();
    void onPage2Clicked();

private:
    Ui::StockTemplatesPageClass ui;

    TruckStockService* truckStockService = nullptr;

    std::vector<StockTemplateDto> currentTemplates;
    std::vector<StockTemplateDto> filteredTemplates;

    int currentPage = 1;
    int pageSize = 10;

    void setupConnections();
    void loadTemplates();
    void filterTemplates();
    void populateTable();
    void updatePagination();

    void addActionButtons(int row, const std::string& templateId);
    void onViewTemplateClicked(const std::string& templateId);
    void onEditTemplateClicked(const std::string& templateId);
    void onDeleteTemplateClicked(const std::string& templateId);
};