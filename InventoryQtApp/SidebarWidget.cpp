#include "SidebarWidget.h"
#include "Theme.h"

#include <algorithm>
#include <QString>


SidebarWidget::SidebarWidget(
    const std::vector<std::string>& permissions,
    const std::string& role,
    const std::string& userName,
    QWidget* parent
)
    : QWidget(parent),
    permissions(permissions),
    role(role),
    userName(userName)
{
    ui.setupUi(this);
    setUserInfo(role, userName);

    connect(ui.dashboardButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.dashboardButton);
        emit dashboardClicked();
        });

    connect(ui.itemsButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.itemsButton);
        emit itemsClicked();
        });

    connect(ui.assetsButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.assetsButton);
        emit assetsClicked();
        });

    connect(ui.usersButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.usersButton);
        emit usersClicked();
        });

    connect(ui.scanInButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.scanInButton);
        emit scanInClicked();
        });

    connect(ui.scanOutButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.scanOutButton);
        emit scanOutClicked();
        });

    connect(ui.reportsButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.reportsButton);
        emit reportsClicked();
        });

    connect(ui.settingsButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.settingsButton);
        emit settingsClicked();
        });

    connect(ui.truckStockDashboardButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.truckStockDashboardButton);
        emit truckStockDashboardClicked();
        });

    connect(ui.fleetButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.fleetButton);
        emit fleetClicked();
        });

    connect(ui.myTruckStockButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.myTruckStockButton);
        emit myTruckStockClicked();
        });

    connect(ui.lowStockAlertsButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.lowStockAlertsButton);
        emit lowStockAlertsClicked();
        });

    connect(ui.receiptsButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.receiptsButton);
        emit receiptsClicked();
        });

    connect(ui.logoutButton, &QPushButton::clicked, this, [this]() {
        emit logoutClicked();
        });

    applyPermissions();
    setActiveButton(ui.dashboardButton);
}



bool SidebarWidget::hasPermission(const std::string& permission) const
{
    return std::find(
        permissions.begin(),
        permissions.end(),
        permission
    ) != permissions.end();
}

void SidebarWidget::applyPermissions()
{
    ui.usersButton->setVisible(hasPermission("MANAGE_USERS"));

    ui.itemsButton->setVisible(
        hasPermission("ADD_PRODUCT") ||
        hasPermission("VIEW_STOCK")
    );

    ui.assetsButton->setVisible(
        hasPermission("VIEW_ASSET") ||
        hasPermission("ADD_ASSET") ||
        hasPermission("EDIT_ASSET") ||
        hasPermission("DELETE_ASSET")
    );

    ui.scanInButton->setVisible(hasPermission("SCAN_IN"));
    ui.scanOutButton->setVisible(hasPermission("SCAN_OUT"));

    ui.truckStockDashboardButton->setVisible(
        hasPermission("VIEW_TRUCK_STOCK") ||
        hasPermission("VIEW_ALL_TRUCKS") ||
        hasPermission("MANAGE_TRUCK_STOCK")
    );

    // Fleet groups Trucks, Templates and Assignments under one tabbed page.
    // Show it to anyone who could reach any of those screens (union of the
    // permissions the three separate nav items used to require).
    ui.fleetButton->setVisible(
        hasPermission("VIEW_ALL_TRUCKS") ||
        hasPermission("MANAGE_TRUCK_STOCK") ||
        hasPermission("ASSIGN_TRUCK_STOCK")
    );

    ui.myTruckStockButton->setVisible(
        hasPermission("VIEW_ASSIGNED_TRUCK_STOCK")
    );

    ui.lowStockAlertsButton->setVisible(
        hasPermission("VIEW_LOW_STOCK_ALERTS")
    );

    ui.receiptsButton->setVisible(
        hasPermission("UPLOAD_RECEIPT") ||
        hasPermission("APPROVE_RECEIPTS")
    );


    // Reports endpoints require VIEW_STOCK (summaries/movements); audit logs
    // additionally require MANAGE_USERS. Mirror the web app, which gates the
    // Reports nav item on VIEW_STOCK. (There is no VIEW_REPORTS permission.)
    ui.reportsButton->setVisible(
        hasPermission("VIEW_STOCK") ||
        hasPermission("MANAGE_USERS")
    );

    // Settings (profile, change password, appearance, sign out) is available to
    // every authenticated user, regardless of role. (There is no
    // MANAGE_SETTINGS permission.)
    ui.settingsButton->setVisible(true);
}

void SidebarWidget::resetButtonStates()
{
    QList<QPushButton*> buttons = findChildren<QPushButton*>();

    for (QPushButton* button : buttons) {
        if (button != ui.logoutButton) {
            button->setChecked(false);
        }
    }
}

void SidebarWidget::setActiveButton(QPushButton* activeButton)
{
    resetButtonStates();

    if (activeButton) {
        activeButton->setChecked(true);
    }
}

void SidebarWidget::setUserInfo(const std::string& role, const std::string& userName)
{
    this->role = role;
    this->userName = userName;

    ui.userNameLabel->setText(
        QString::fromStdString(userName)
    );

    ui.userRoleLabel->setText(
        QString::fromStdString(role)
    );
}


void SidebarWidget::activateDashboard()
{
    setActiveButton(ui.dashboardButton);
}

void SidebarWidget::activateItems()
{
    setActiveButton(ui.itemsButton);
}

void SidebarWidget::activateAssets()
{
    setActiveButton(ui.assetsButton);
}

void SidebarWidget::activateUsers()
{
    setActiveButton(ui.usersButton);
}

void SidebarWidget::activateScanIn()
{
    setActiveButton(ui.scanInButton);
}

void SidebarWidget::activateScanOut()
{
    setActiveButton(ui.scanOutButton);
}

void SidebarWidget::activateReports()
{
    setActiveButton(ui.reportsButton);
}

void SidebarWidget::activateSettings()
{
    setActiveButton(ui.settingsButton);
}

void SidebarWidget::activateTruckStockDashboard()
{
    setActiveButton(ui.truckStockDashboardButton);
}

void SidebarWidget::activateFleet()
{
    setActiveButton(ui.fleetButton);
}

void SidebarWidget::activateMyTruckStock()
{
    setActiveButton(ui.myTruckStockButton);
}

void SidebarWidget::activateLowStockAlerts()
{
    setActiveButton(ui.lowStockAlertsButton);
}

void SidebarWidget::activateReceipts()
{
    setActiveButton(ui.receiptsButton);
}

void SidebarWidget::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::sidebarStyle(theme)
    );
}

