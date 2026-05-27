#pragma once

#include <QWidget>
#include <QString>
#include <vector>

#include "ui_TrucksPage.h"
#include "TruckStockService.h"
#include "UserService.h"

class TrucksPage : public QWidget
{
    Q_OBJECT

public:
    TrucksPage(
        TruckStockService* truckStockService,
        UserService* userService,
        QWidget* parent = nullptr
    );

    ~TrucksPage();

    void refreshTrucksList();

private slots:
    void onAddTruckClicked();
    void onSearchChanged(const QString& text);

    void onPreviousPageClicked();
    void onNextPageClicked();
    void onPage2Clicked();

private:
    Ui::TrucksPageClass ui;

    TruckStockService* truckStockService = nullptr;
    UserService* userService = nullptr;

    std::vector<TruckDto> currentTrucks;
    std::vector<TruckDto> filteredTrucks;

    int currentPage = 1;
    int pageSize = 10;

    void setupConnections();
    void loadTrucks();

    void filterTrucks();
    void populateTable();
    void updatePagination();

    void addActionButtons(int row, const std::string& truckId);
    void onEditTruckClicked(const std::string& truckId);
    void onDeleteTruckClicked(const std::string& truckId);
};