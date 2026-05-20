#pragma once

#include <QWidget>
#include "ui_StockTemplatesPage.h"

class StockTemplatesPage : public QWidget
{
    Q_OBJECT

public:
    StockTemplatesPage(QWidget* parent = nullptr);
    ~StockTemplatesPage();

    void refreshTemplatesList();

private slots:
    void onNewTemplateClicked();
    void onSearchChanged(const QString& text);
    void onPageChanged(int page);

private:
    Ui::StockTemplatesPageClass ui;

    void setupConnections();
    void loadTemplates();
};
