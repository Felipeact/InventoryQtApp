#pragma once

#include <QWidget>
#include <QString>

#include "ui_AssignmentsPage.h"
#include "TruckStockService.h"
#include "UserService.h"

class AssignmentsPage : public QWidget
{
public:
    explicit AssignmentsPage(
        TruckStockService* truckStockService,
        UserService* userService,
        QWidget* parent = nullptr
    );

    ~AssignmentsPage();

    void refreshAssignments();

private:
    Ui::AssignmentsPageClass ui;
    TruckStockService* truckStockService = nullptr;
    UserService* userService = nullptr;

    void setupConnections();
    void loadAssignments();

    void onAssignTemplateClicked();
    void onPageChanged(int page);
};