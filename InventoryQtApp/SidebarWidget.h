#pragma once

#include <QWidget>
#include <QPushButton>
#include <vector>
#include <string>

#include "ui_SidebarWidget.h"
#include "Theme.h"

class SidebarWidget : public QWidget
{
    Q_OBJECT

public:

    void applyTheme(Theme::AppTheme theme);
    explicit SidebarWidget(const std::vector<std::string>& permissions, const std::string& role, const std::string& userName, QWidget* parent = nullptr);

    void setUserInfo(const std::string& role, const std::string& userName);


signals:
    void dashboardClicked();
    void itemsClicked();
    void assetsClicked();
    void usersClicked();
    void scanInClicked();
    void scanOutClicked();
    void reportsClicked();
    void settingsClicked();

    void truckStockDashboardClicked();
    void trucksClicked();
    void templatesClicked();
    void assignmentsClicked();
    void myTruckStockClicked();
    void lowStockAlertsClicked();
    void receiptsClicked();

    void logoutClicked();

private:
    Ui::SidebarWidgetClass ui;
    std::vector<std::string> permissions;
    std::string role;
    std::string userName;

    bool hasPermission(const std::string& permission) const;
    void applyPermissions();

    void resetButtonStates();
    void setActiveButton(QPushButton* activeButton);
};