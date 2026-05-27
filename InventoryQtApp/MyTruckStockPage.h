#pragma once

#include <QWidget>
#include <QString>

#include "ui_MyTruckStockPage.h"
#include "TruckStockService.h"

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

private slots:
    void onSearchChanged(const QString& text);
    void addUseButton(int row);
    void onUseItemClicked(int row);

private:
    Ui::MyTruckStockPageClass ui;

    TruckStockService* truckStockService = nullptr;
    MyTruckStockDto currentStock;

    void setupConnections();
    void loadStock();

    
};