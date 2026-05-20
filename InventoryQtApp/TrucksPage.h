#pragma once

#include <QWidget>
#include "ui_TrucksPage.h"

class TrucksPage : public QWidget
{
    Q_OBJECT

public:
    TrucksPage(QWidget* parent = nullptr);
    ~TrucksPage();

    void refreshTrucksList();

private slots:
    void onAddTruckClicked();
    void onSearchChanged(const QString& text);
    void onPageChanged(int page);

private:
    Ui::TrucksPageClass ui;

    void setupConnections();
    void loadTrucks();
};
