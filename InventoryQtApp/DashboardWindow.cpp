// DashboardWindow.cpp - Implementation of the main dashboard window
#include "DashboardWindow.h"
#include "Theme.h"

#include <QVBoxLayout>
#include <QString>
#include <QSettings>

// Constructor initializes the main window with pages and navigation
DashboardWindow::DashboardWindow(
    const std::string& role,
    const std::vector<std::string>& permissions,
    const std::string& userName,
    ProductService& productService,
    AssetService& assetService,
    UserService& userService,
    ReportService& reportService,
    TruckStockService& truckStockService,
    QWidget* parent
)
    : QMainWindow(parent),
    role(role),
    permissions(permissions),
    userName(userName),
    productService(&productService),
    assetService(&assetService),
    userService(&userService),
    truckStockService(&truckStockService),
    reportService(&reportService)
{
    ui.setupUi(this);

    this->setWindowTitle("Inventory Dashboard");
    this->resize(1440, 900);
    this->setMinimumSize(1100, 720);

    setupPages();
    setupSidebar();
    setupVerticalbar();

    ui.mainStack->setCurrentWidget(dashboardPage);
}

DashboardWindow::~DashboardWindow()
{
}

void DashboardWindow::updateLoggedInUserInfo(
    const std::string& newUserName
)
{
    userName = newUserName;

    if (verticalbar) {
        verticalbar->setUserInfo(role, userName);
    }

    if (sidebar) {
        sidebar->setUserInfo(role, userName);
    }
}

void DashboardWindow::setupPages()
{
    dashboardPage = new DashboardPage(*productService, *reportService, this);
    ui.mainStack->addWidget(dashboardPage);

    itemsPage = new ItemsPage(*productService, this);
    ui.mainStack->addWidget(itemsPage);

    assetsPage = new AssetsPage(*assetService, this);
    ui.mainStack->addWidget(assetsPage);

    usersPage = new UsersPage(userService, this);
    ui.mainStack->addWidget(usersPage);

    usersPage->setLoggedInUserName(userName);

    connect(
        usersPage,
        &UsersPage::loggedInUserUpdated,
        this,
        &DashboardWindow::updateLoggedInUserInfo
    );

    scanInPage = new ScanPage(*productService, ScanMode::ScanIn, this);
    ui.mainStack->addWidget(scanInPage);

    scanOutPage = new ScanPage(*productService, ScanMode::ScanOut, this);
    ui.mainStack->addWidget(scanOutPage);

    connect(dashboardPage, &DashboardPage::viewAllItemsRequested, this, [this]() {
        ui.mainStack->setCurrentWidget(itemsPage);
        });

    connect(itemsPage, &ItemsPage::productsChanged, this, [this]() {
        dashboardPage->refreshDashboard();
        });

    connect(assetsPage, &AssetsPage::assetsChanged, this, [this]() {
        dashboardPage->refreshDashboard();
        });

    connect(scanInPage, &ScanPage::stockChanged, this, [this]() {
        dashboardPage->refreshDashboard();
        itemsPage->refreshProducts();
        });

    connect(scanOutPage, &ScanPage::stockChanged, this, [this]() {
        dashboardPage->refreshDashboard();
        itemsPage->refreshProducts();
        });

    settingsPage = new SettingsPage(role, userName, this);
    ui.mainStack->addWidget(settingsPage);

    connect(
        settingsPage,
        &SettingsPage::userNameChanged,
        this,
        &DashboardWindow::updateLoggedInUserInfo
    );

    connect(
        settingsPage,
        &SettingsPage::themeChanged,
        this,
        [this](const QString& themeName) {
            QSettings settings("InventorySystem", "InventoryQtApp");
            settings.setValue("appearance/theme", themeName);

            Theme::AppTheme appTheme =
                themeName == "Light"
                ? Theme::AppTheme::Light
                : Theme::AppTheme::Dark;

            Theme::applyTheme(appTheme);
            applyTheme(appTheme);

            if (dashboardPage) dashboardPage->applyTheme(appTheme);
            if (itemsPage) itemsPage->applyTheme(appTheme);
            if (assetsPage) assetsPage->applyTheme(appTheme);
            if (usersPage) usersPage->applyTheme(appTheme);
            if (settingsPage) settingsPage->applyTheme(appTheme);
            if (sidebar) sidebar->applyTheme(appTheme);
            if (verticalbar) verticalbar->applyTheme(appTheme);
            if (scanInPage) scanInPage->applyTheme(appTheme);
            if (scanOutPage) scanOutPage->applyTheme(appTheme);
        }
    );

    connect(
        settingsPage,
        &SettingsPage::logoutRequested,
        this,
        [this]() {
            emit logoutRequested();
            this->close();
        }
    );

    truckStockDashboardPage = new TruckStockDashboardPage(truckStockService, this);
    ui.mainStack->addWidget(truckStockDashboardPage);

    trucksPage = new TrucksPage(truckStockService, userService, this);
    ui.mainStack->addWidget(trucksPage);

    stockTemplatesPage = new StockTemplatesPage(truckStockService, this);
    ui.mainStack->addWidget(stockTemplatesPage);

    assignmentsPage = new AssignmentsPage(truckStockService, userService, this);
    ui.mainStack->addWidget(assignmentsPage);

    myTruckStockPage = new MyTruckStockPage(truckStockService, this);
    ui.mainStack->addWidget(myTruckStockPage);

    lowStockAlertsPage = new LowStockAlertsPage(truckStockService, this);
    ui.mainStack->addWidget(lowStockAlertsPage);

    receiptsPage = new ReceiptsPage(truckStockService, permissions, this);
    ui.mainStack->addWidget(receiptsPage);

    QSettings settings("InventorySystem", "InventoryQtApp");

    QString savedTheme =
        settings.value("appearance/theme", "Dark").toString();

    Theme::AppTheme appTheme =
        savedTheme == "Light"
        ? Theme::AppTheme::Light
        : Theme::AppTheme::Dark;

    Theme::applyTheme(appTheme);

    applyTheme(appTheme);

    dashboardPage->applyTheme(appTheme);
    itemsPage->applyTheme(appTheme);
    assetsPage->applyTheme(appTheme);
    usersPage->applyTheme(appTheme);

    scanInPage->applyTheme(appTheme);
    scanOutPage->applyTheme(appTheme);

    settingsPage->applyTheme(appTheme);

    if (sidebar)
        sidebar->applyTheme(appTheme);

    if (verticalbar)
        verticalbar->applyTheme(appTheme);
}

void DashboardWindow::setupSidebar()
{
    sidebar = new SidebarWidget(permissions, role, userName, this);

    QVBoxLayout* layout = new QVBoxLayout(ui.sidebarContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(sidebar);

    connect(sidebar, &SidebarWidget::dashboardClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(dashboardPage);
        });

    connect(sidebar, &SidebarWidget::itemsClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(itemsPage);
        });

    connect(sidebar, &SidebarWidget::assetsClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(assetsPage);
        });

    connect(sidebar, &SidebarWidget::usersClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(usersPage);
        });

    connect(sidebar, &SidebarWidget::scanInClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(scanInPage);
        });

    connect(sidebar, &SidebarWidget::scanOutClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(scanOutPage);
        });

    connect(sidebar, &SidebarWidget::settingsClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(settingsPage);
        });

    connect(sidebar, &SidebarWidget::truckStockDashboardClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(truckStockDashboardPage);
        });

    connect(sidebar, &SidebarWidget::trucksClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(trucksPage);
        });

    connect(sidebar, &SidebarWidget::templatesClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(stockTemplatesPage);
        });

    connect(sidebar, &SidebarWidget::assignmentsClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(assignmentsPage);
        });

    connect(sidebar, &SidebarWidget::myTruckStockClicked, this, [this]() {
        myTruckStockPage->refreshStock();
        ui.mainStack->setCurrentWidget(myTruckStockPage);
        });

    connect(sidebar, &SidebarWidget::lowStockAlertsClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(lowStockAlertsPage);
        });

    connect(sidebar, &SidebarWidget::receiptsClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(receiptsPage);
        });

    connect(sidebar, &SidebarWidget::logoutClicked, this, [this]() {
        emit logoutRequested();
        this->close();
        });

    QSettings settings("InventorySystem", "InventoryQtApp");

    QString savedTheme =
        settings.value("appearance/theme", "Dark").toString();

    Theme::AppTheme appTheme =
        savedTheme == "Light"
        ? Theme::AppTheme::Light
        : Theme::AppTheme::Dark;

    sidebar->applyTheme(appTheme);
}

void DashboardWindow::setupVerticalbar()
{
    verticalbar = new VerticalWidget(role, userName, this);

    QVBoxLayout* layout = new QVBoxLayout(ui.verticalContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(verticalbar);

    QSettings settings("InventorySystem", "InventoryQtApp");

    QString savedTheme =
        settings.value("appearance/theme", "Dark").toString();

    Theme::AppTheme appTheme =
        savedTheme == "Light"
        ? Theme::AppTheme::Light
        : Theme::AppTheme::Dark;

    verticalbar->applyTheme(appTheme);
}

void DashboardWindow::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::shellStyle(theme)
    );
}
