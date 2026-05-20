#pragma once

#include <QWidget>
#include "ui_AssignmentsPage.h"

class AssignmentsPage : public QWidget
{
    Q_OBJECT

public:
    AssignmentsPage(QWidget* parent = nullptr);
    ~AssignmentsPage();

    void refreshAssignments();

private slots:
    void onAssignTemplateClicked();
    void onPageChanged(int page);

private:
    Ui::AssignmentsPageClass ui;

    void setupConnections();
    void loadAssignments();
};
