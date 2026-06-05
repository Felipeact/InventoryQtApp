#pragma once

#include <QWidget>
#include <QString>
#include <vector>

#include "ui_MyTruckStockPage.h"
#include "TruckStockService.h"
#include "Theme.h"

class MyTruckStockPage : public QWidget
{
    Q_OBJECT

public:
    explicit MyTruckStockPage(
        TruckStockService* truckStockService,
        QWidget* parent = nullptr
    );

    ~MyTruckStockPage();

    void refreshStock();

    void applyTheme(
        Theme::AppTheme theme
    );

    void setSearchText(const QString& text);

private slots:
    void onSearchChanged(const QString& text);
    void onUseItemClicked(int row);

signals:
    void stockChanged();

private:
    Ui::MyTruckStockPageClass ui;

    TruckStockService* truckStockService = nullptr;
    MyTruckStockDto currentStock;
    std::vector<MyTruckStockItemDto> filteredItems;

    void setupConnections();
    void setupTable();
    void loadStock();

    void addUseButton(int row);

    void filterStock();
    void populateTable();
};