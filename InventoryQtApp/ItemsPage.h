#pragma once

#include <QWidget>
#include "ui_ItemsPage.h"

class ItemsPage : public QWidget
{
    Q_OBJECT

public:
    ItemsPage(QWidget* parent = nullptr);
    ~ItemsPage();

private:
    Ui::ItemsPageClass ui;

    void setupTable();
    void loadFakeData();
};