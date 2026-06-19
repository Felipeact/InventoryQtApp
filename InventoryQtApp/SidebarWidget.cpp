#include "SidebarWidget.h"
#include "Theme.h"

#include <algorithm>
#include <QString>
#include <QColor>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QSize>
#include <QSvgRenderer>


namespace {

// Minimal line-icon set (Lucide-style, 24x24 stroke geometry). Only the inner
// shapes are stored; makeNavIcon() wraps them in an <svg> with the current
// theme's stroke colour so a single source recolours for light/dark and stays
// legible in both the normal and selected (checked) button states.
const char* const kIconHome      = R"(<path d='M3 9l9-7 9 7v11a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2z'/><path d='M9 22V12h6v10'/>)";
const char* const kIconPackage   = R"(<path d='M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z'/><path d='M3.27 6.96 12 12.01l8.73-5.05'/><path d='M12 22.08V12'/>)";
const char* const kIconWrench    = R"(<path d='M14.7 6.3a1 1 0 0 0 0 1.4l1.6 1.6a1 1 0 0 0 1.4 0l3.77-3.77a6 6 0 0 1-7.94 7.94l-6.91 6.91a2.12 2.12 0 0 1-3-3l6.91-6.91a6 6 0 0 1 7.94-7.94l-3.76 3.76z'/>)";
const char* const kIconUsers     = R"(<path d='M16 21v-2a4 4 0 0 0-4-4H6a4 4 0 0 0-4 4v2'/><circle cx='9' cy='7' r='4'/><path d='M22 21v-2a4 4 0 0 0-3-3.87'/><path d='M16 3.13a4 4 0 0 1 0 7.75'/>)";
const char* const kIconDownload  = R"(<path d='M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4'/><polyline points='7 10 12 15 17 10'/><line x1='12' y1='15' x2='12' y2='3'/>)";
const char* const kIconUpload    = R"(<path d='M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4'/><polyline points='17 8 12 3 7 8'/><line x1='12' y1='3' x2='12' y2='15'/>)";
const char* const kIconBarChart  = R"(<path d='M3 3v18h18'/><path d='M18 17V9'/><path d='M13 17V5'/><path d='M8 17v-3'/>)";
const char* const kIconSliders   = R"(<line x1='4' y1='21' x2='4' y2='14'/><line x1='4' y1='10' x2='4' y2='3'/><line x1='12' y1='21' x2='12' y2='12'/><line x1='12' y1='8' x2='12' y2='3'/><line x1='20' y1='21' x2='20' y2='16'/><line x1='20' y1='12' x2='20' y2='3'/><line x1='2' y1='14' x2='6' y2='14'/><line x1='10' y1='8' x2='14' y2='8'/><line x1='18' y1='16' x2='22' y2='16'/>)";
const char* const kIconGauge     = R"(<path d='M12 14l4-4'/><path d='M3.34 19a10 10 0 1 1 17.32 0'/>)";
const char* const kIconTruck     = R"(<path d='M5 18H3c-.6 0-1-.4-1-1V7c0-.6.4-1 1-1h10c.6 0 1 .4 1 1v11'/><path d='M14 9h4l4 4v4c0 .6-.4 1-1 1h-2'/><circle cx='7' cy='18' r='2'/><path d='M15 18H9'/><circle cx='17' cy='18' r='2'/>)";
const char* const kIconClipboard = R"(<rect x='8' y='2' width='8' height='4' rx='1'/><path d='M16 4h2a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h2'/><path d='M12 11h4'/><path d='M12 16h4'/><path d='M8 11h.01'/><path d='M8 16h.01'/>)";
const char* const kIconAlert     = R"(<path d='M10.29 3.86 1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z'/><line x1='12' y1='9' x2='12' y2='13'/><line x1='12' y1='17' x2='12.01' y2='17'/>)";
const char* const kIconReceipt   = R"(<path d='M15 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V7z'/><path d='M14 2v5h5'/><line x1='16' y1='13' x2='8' y2='13'/><line x1='16' y1='17' x2='8' y2='17'/><line x1='10' y1='9' x2='8' y2='9'/>)";
const char* const kIconLogout    = R"(<path d='M9 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h4'/><polyline points='16 17 21 12 16 7'/><line x1='21' y1='12' x2='9' y2='12'/>)";

QIcon makeNavIcon(const QString& body, const QColor& color)
{
    const QString svg = QStringLiteral(
        "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='none' "
        "stroke='%1' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'>%2</svg>"
    ).arg(color.name(), body);

    QSvgRenderer renderer(svg.toUtf8());

    const int side = 20;
    const qreal dpr = 2.0;
    QPixmap pm(int(side * dpr), int(side * dpr));
    pm.setDevicePixelRatio(dpr);
    pm.fill(Qt::transparent);

    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    renderer.render(&painter);
    painter.end();

    return QIcon(pm);
}

} // namespace

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

    // Recolour the nav icons for the active theme. One slate tone per theme
    // stays legible on both the normal background and the indigo selected state.
    const QColor iconColor = (theme == Theme::AppTheme::Light)
        ? QColor("#475569")
        : QColor("#C7D2E4");
    const QColor logoutColor("#DC2626");
    const QSize iconSize(20, 20);

    auto apply = [&](QPushButton* button, const QString& body,
                     const QString& label, const QColor& color) {
        if (!button) {
            return;
        }
        button->setIcon(makeNavIcon(body, color));
        button->setIconSize(iconSize);
        button->setText(label);
    };

    apply(ui.dashboardButton, kIconHome, QStringLiteral("Dashboard"), iconColor);
    apply(ui.itemsButton, kIconPackage, QStringLiteral("Items"), iconColor);
    apply(ui.assetsButton, kIconWrench, QStringLiteral("Assets"), iconColor);
    apply(ui.usersButton, kIconUsers, QStringLiteral("Users"), iconColor);
    apply(ui.scanInButton, kIconDownload, QStringLiteral("Scan In"), iconColor);
    apply(ui.scanOutButton, kIconUpload, QStringLiteral("Scan Out"), iconColor);
    apply(ui.reportsButton, kIconBarChart, QStringLiteral("Reports"), iconColor);
    apply(ui.settingsButton, kIconSliders, QStringLiteral("Settings"), iconColor);
    apply(ui.truckStockDashboardButton, kIconGauge, QStringLiteral("Truck Dashboard"), iconColor);
    apply(ui.fleetButton, kIconTruck, QStringLiteral("Fleet"), iconColor);
    apply(ui.myTruckStockButton, kIconClipboard, QStringLiteral("My Truck Stock"), iconColor);
    apply(ui.lowStockAlertsButton, kIconAlert, QStringLiteral("Low Stock"), iconColor);
    apply(ui.receiptsButton, kIconReceipt, QStringLiteral("Receipts"), iconColor);
    apply(ui.logoutButton, kIconLogout, QStringLiteral("Logout"), logoutColor);
}

