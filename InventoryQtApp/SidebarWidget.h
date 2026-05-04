// SidebarWidget.h - Navigation sidebar for application pages
#pragma once

#include <QWidget>
#include <vector>
#include <string>
#include "ui_SidebarWidget.h"

// Widget providing navigation buttons with permission-based visibility
class SidebarWidget : public QWidget
{
    Q_OBJECT

public:
    // Constructor that initializes the sidebar with user permissions
    SidebarWidget(
        const std::vector<std::string>& permissions,
        QWidget* parent = nullptr
    );

signals:
    // Navigation signals emitted when buttons are clicked
    void dashboardClicked();
    void itemsClicked();
    void assetsClicked();
    void usersClicked();
    void scanInClicked();
    void scanOutClicked();
    void logoutClicked();

private:
    Ui::SidebarWidgetClass ui;                 // UI components
    std::vector<std::string> permissions;     // User permissions

    // Checks if user has a specific permission
    bool hasPermission(const std::string& permission);
    // Shows/hides buttons based on user permissions
    void applyPermissions();
};
