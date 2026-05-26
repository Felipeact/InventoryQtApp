#pragma once

#include <QWidget>
#include <QString>

#include "ui_AssignmentsPage.h"
#include "TruckStockService.h"

class AssignmentsPage : public QWidget
{
public:
    explicit AssignmentsPage(
        TruckStockService* truckStockService,
        QWidget* parent = nullptr
    );

    ~AssignmentsPage();

    void refreshAssignments();

private:
    Ui::AssignmentsPageClass ui;
    TruckStockService* truckStockService = nullptr;

    void setupConnections();
    void loadAssignments();

    void onAssignTemplateClicked();
    void onPageChanged(int page);
};