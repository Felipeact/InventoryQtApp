// SidebarWidget.cpp - Implementation of the navigation sidebar
#include "SidebarWidget.h"
#include <algorithm>

// Constructor initializes sidebar buttons and applies permission-based visibility
SidebarWidget::SidebarWidget(
    const std::vector<std::string>& userPermissions,
    QWidget* parent
)
    : QWidget(parent),
    permissions(userPermissions)
{
    ui.setupUi(this);

    applyPermissions();

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

    connect(ui.scanInButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.scanInButton);
        emit scanInClicked();
        });

    connect(ui.scanOutButton, &QPushButton::clicked, this, [this]() {
        setActiveButton(ui.scanOutButton);
        emit scanOutClicked();
        });

    connect(ui.logoutButton, &QPushButton::clicked, this, &SidebarWidget::logoutClicked);

    setActiveButton(ui.dashboardButton);
}

// Checks if the user has a specific permission
bool SidebarWidget::hasPermission(const std::string& permission)
{
    return std::find(
        permissions.begin(),
        permissions.end(),
        permission
    ) != permissions.end();
}

// Configures button visibility based on user permissions
void SidebarWidget::applyPermissions()
{
    ui.itemsButton->setVisible(hasPermission("ADD_PRODUCT"));
    ui.assetsButton->setVisible(hasPermission("VIEW_STOCK"));
    ui.scanInButton->setVisible(hasPermission("SCAN_IN"));
    ui.scanOutButton->setVisible(hasPermission("SCAN_OUT"));
    ui.usersButton->setVisible(hasPermission("MANAGE_USERS"));
}

void SidebarWidget::resetButtonStates()
{
    QString normalStyle = R"(
        QPushButton {
            background-color: transparent;
            color: white;
            border: none;
            border-radius: 10px;
            text-align: left;
            padding-left: 18px;
            font-size: 14px;
            font-weight: 600;
        }

        QPushButton:hover {
            background-color: #132238;
        }
    )";

    ui.dashboardButton->setStyleSheet(normalStyle);
    ui.itemsButton->setStyleSheet(normalStyle);
    ui.assetsButton->setStyleSheet(normalStyle);
    ui.usersButton->setStyleSheet(normalStyle);
    ui.scanInButton->setStyleSheet(normalStyle);
    ui.scanOutButton->setStyleSheet(normalStyle);
}

void SidebarWidget::setActiveButton(QPushButton* activeButton)
{
    resetButtonStates();

    QString activeStyle = R"(
        QPushButton {
            background-color: #2563EB;
            color: white;
            border: none;
            border-radius: 10px;
            text-align: left;
            padding-left: 18px;
            font-size: 14px;
            font-weight: 700;
        }
    )";

    activeButton->setStyleSheet(activeStyle);
}
