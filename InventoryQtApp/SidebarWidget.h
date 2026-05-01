#pragma once

#include <QWidget>
#include <vector>
#include <string>
#include "ui_SidebarWidget.h"

class SidebarWidget : public QWidget
{
    Q_OBJECT

public:
    SidebarWidget(
        const std::vector<std::string>& permissions,
        QWidget* parent = nullptr
    );

signals:
    void dashboardClicked();
    void itemsClicked();
    void assetsClicked();
    void usersClicked();
    void scanInClicked();
    void scanOutClicked();
    void logoutClicked();

private:
    Ui::SidebarWidgetClass ui;
    std::vector<std::string> permissions;

    bool hasPermission(const std::string& permission);
    void applyPermissions();
};
