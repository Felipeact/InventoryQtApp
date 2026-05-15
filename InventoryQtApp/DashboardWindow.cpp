// DashboardWindow.cpp - Implementation of the main dashboard window
#include "DashboardWindow.h"

#include <QVBoxLayout>
#include <QString>

// Constructor initializes the main window with pages and navigation
DashboardWindow::DashboardWindow(
    const std::string& role,
    const std::vector<std::string>& permissions,
    ProductService& productService,
    AssetService& assetService,
    ReportService& reportService,
    QWidget* parent
)
    : QMainWindow(parent),
    role(role),
    permissions(permissions),
    productService(&productService),
    assetService(&assetService),
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

void DashboardWindow::setupPages()
{
    dashboardPage = new DashboardPage(*productService, *reportService, this);
    ui.mainStack->addWidget(dashboardPage);

    itemsPage = new ItemsPage(*productService, this);
    ui.mainStack->addWidget(itemsPage);

    assetsPage = new AssetsPage(*assetService, this);
    ui.mainStack->addWidget(assetsPage);

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
}

void DashboardWindow::setupSidebar()
{
    sidebar = new SidebarWidget(permissions, this);

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

    connect(sidebar, &SidebarWidget::scanInClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(scanInPage);
        });

    connect(sidebar, &SidebarWidget::scanOutClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(scanOutPage);
        });

    connect(sidebar, &SidebarWidget::logoutClicked, this, [this]() {
        emit logoutRequested();
        this->close();
        });
}

void DashboardWindow::setupVerticalbar()
{
    verticalbar = new VerticalWidget(role, "Felipe", this);

    QVBoxLayout* layout = new QVBoxLayout(ui.verticalContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(verticalbar);
}