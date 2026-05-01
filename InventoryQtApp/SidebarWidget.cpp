#include "SidebarWidget.h"
#include <algorithm>

SidebarWidget::SidebarWidget(
    const std::vector<std::string>& userPermissions,
    QWidget* parent
)
    : QWidget(parent),
    permissions(userPermissions)
{
    ui.setupUi(this);

    applyPermissions();

    connect(ui.dashboardButton, &QPushButton::clicked, this, &SidebarWidget::dashboardClicked);
    connect(ui.itemsButton, &QPushButton::clicked, this, &SidebarWidget::itemsClicked);
    connect(ui.assetsButton, &QPushButton::clicked, this, &SidebarWidget::assetsClicked);
    connect(ui.usersButton, &QPushButton::clicked, this, &SidebarWidget::usersClicked);
    connect(ui.scanInButton, &QPushButton::clicked, this, &SidebarWidget::scanInClicked);
    connect(ui.scanOutButton, &QPushButton::clicked, this, &SidebarWidget::scanOutClicked);
    connect(ui.logoutButton, &QPushButton::clicked, this, &SidebarWidget::logoutClicked);
}

bool SidebarWidget::hasPermission(const std::string& permission)
{
    return std::find(
        permissions.begin(),
        permissions.end(),
        permission
    ) != permissions.end();
}

void SidebarWidget::applyPermissions()
{
    ui.itemsButton->setVisible(hasPermission("ADD_PRODUCT"));
    ui.assetsButton->setVisible(hasPermission("VIEW_STOCK"));
    ui.scanInButton->setVisible(hasPermission("SCAN_IN"));
    ui.scanOutButton->setVisible(hasPermission("SCAN_OUT"));
    ui.usersButton->setVisible(hasPermission("MANAGE_USERS"));
}