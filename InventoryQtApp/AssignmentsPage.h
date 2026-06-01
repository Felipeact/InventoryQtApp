#pragma once

#include <QWidget>
#include <vector>

#include "ui_AssignmentsPage.h"
#include "TruckStockService.h"
#include "UserService.h"
#include "Theme.h"

class AssignmentsPage : public QWidget
{
    Q_OBJECT

public:

    void applyTheme(Theme::AppTheme theme);
    explicit AssignmentsPage(
        TruckStockService* truckStockService,
        UserService* userService,
        QWidget* parent = nullptr
    );

    ~AssignmentsPage();

    void refreshAssignments();

private slots:
    void onAssignTemplateClicked();

    void onPreviousPageClicked();
    void onNextPageClicked();
    void onPage2Clicked();

private:
    Ui::AssignmentsPageClass ui;

    TruckStockService* truckStockService = nullptr;
    UserService* userService = nullptr;

    std::vector<TruckAssignmentDto> currentAssignments;

    int currentPage = 1;
    int pageSize = 10;

    void setupConnections();
    void loadAssignments();
    void populateTable();
    void updatePagination();
};