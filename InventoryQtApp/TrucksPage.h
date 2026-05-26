#pragma once

#include <QWidget>
#include "ui_TrucksPage.h"
#include "TruckStockService.h"
#include "UserService.h"

class TrucksPage : public QWidget
{
    Q_OBJECT

public:
    TrucksPage(TruckStockService* truckStockService, UserService* userService, QWidget* parent = nullptr);
    ~TrucksPage();

    void refreshTrucksList();

private slots:
    void onAddTruckClicked();
    void onSearchChanged(const QString& text);
    void onPageChanged(int page);

    void addActionButtons(int row);
    void onEditTruckClicked(int row);
    void onDeleteTruckClicked(int row);

private:
    Ui::TrucksPageClass ui;

    TruckStockService* truckStockService = nullptr;
    UserService* userService = nullptr;
    std::vector<TruckDto> currentTrucks;

    void setupConnections();
    void loadTrucks();
};
