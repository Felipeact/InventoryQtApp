#pragma once

#include <QWidget>
#include "ui_MyTruckStockPage.h"

class MyTruckStockPage : public QWidget
{
    Q_OBJECT

public:
    MyTruckStockPage(QWidget* parent = nullptr);
    ~MyTruckStockPage();

    void refreshStock();

private slots:
    void onTruckSelected(int index);
    void onSearchChanged(const QString& text);
    void onUseItemClicked();

private:
    Ui::MyTruckStockPageClass ui;

    void setupConnections();
    void loadTrucks();
    void loadStock();
};
