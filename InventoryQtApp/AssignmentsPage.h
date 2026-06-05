#pragma once

#include <QWidget>
#include <QString>
#include <vector>

#include "ui_AssignmentsPage.h"
#include "TruckStockService.h"
#include "UserService.h"
#include "Theme.h"

class AssignmentsPage : public QWidget
{
    Q_OBJECT

public:
    explicit AssignmentsPage(
        TruckStockService* truckStockService,
        UserService* userService,
        QWidget* parent = nullptr
    );

    ~AssignmentsPage();

    void refreshAssignments();

    void applyTheme(
        Theme::AppTheme theme
    );

    void filterAssignments();

private slots:
    void onAssignTemplateClicked();

    void onPreviousPageClicked();
    void onNextPageClicked();
    void onPage2Clicked();
    void onSearchChanged(const QString& text);

signals:
    void assignmentsChanged();

private:
    Ui::AssignmentsPageClass ui;

    TruckStockService* truckStockService = nullptr;
    UserService* userService = nullptr;

    std::vector<TruckAssignmentDto> currentAssignments;
    std::vector<TruckAssignmentDto> filteredAssignments;

    int currentPage = 1;
    int pageSize = 10;

    void setupConnections();
    void setupTable();

    void loadAssignments();
    void populateTable();
    void updatePagination();
};