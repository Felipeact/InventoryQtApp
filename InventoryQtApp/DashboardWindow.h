#pragma once
#include <DashboardPage.h>
#include <ItemsPage.h>

#include <QMainWindow>
#include <vector>
#include <string>

#include "ui_DashboardWindow.h"
#include "SidebarWidget.h"
#include "VerticalWidget.h"

class DashboardWindow : public QMainWindow
{
    Q_OBJECT

public:
    DashboardWindow(
        const std::string& role,
        const std::vector<std::string>& permissions,
        QWidget* parent = nullptr
    );

    ~DashboardWindow();

private:
    Ui::DashboardWindowClass ui;

    std::string role;
    std::vector<std::string> permissions;

    SidebarWidget* sidebar = nullptr;
	VerticalWidget* verticalbar = nullptr;
    
    // Add Pages to the stack
	DashboardPage* dashboardPage = nullptr;
	ItemsPage* itemsPage = nullptr;

    void setupSidebar();
	void setupVerticalbar();
    void setupDashboardPage();


};