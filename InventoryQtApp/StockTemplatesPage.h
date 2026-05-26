#pragma once

#include <QWidget>
#include "ui_StockTemplatesPage.h"

#include "TruckStockService.h"
#include <vector>

class StockTemplatesPage : public QWidget
{
    Q_OBJECT

public:
    StockTemplatesPage(TruckStockService* truckStockService, QWidget* parent = nullptr);
    ~StockTemplatesPage();

    void refreshTemplatesList();

private slots:
    void onNewTemplateClicked();
    void onSearchChanged(const QString& text);
    void onPageChanged(int page);

private:
    Ui::StockTemplatesPageClass ui;

    TruckStockService* truckStockService = nullptr;
    std::vector<StockTemplateDto> currentTemplates;

    void setupConnections();
    void loadTemplates();
    void refreshTemplates();

    void addActionButtons(int row);
    void onViewTemplateClicked(int row);
    void onEditTemplateClicked(int row);
    void onDeleteTemplateClicked(int row);


};
