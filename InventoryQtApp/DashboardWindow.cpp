// DashboardWindow.cpp - Implementation of the main dashboard window
#include "DashboardWindow.h"

#include <QVBoxLayout>
#include <QString>
#include <QTableWidgetItem>
#include <QHeaderView>

// Constructor initializes the main window with pages and navigation
DashboardWindow::DashboardWindow(const std::string& role, const std::vector<std::string>& permissions, ProductService& productService, AssetService& assetService, ReportService& reportService, QWidget* parent) :
    QMainWindow(parent), role(role), permissions(permissions), productService(&productService), assetService(&assetService), reportService(&reportService)   
{
    ui.setupUi(this);

    dashboardPage = new DashboardPage(productService, reportService, this);
    ui.mainStack->addWidget(dashboardPage);

    itemsPage = new ItemsPage(productService, this);
    ui.mainStack->addWidget(itemsPage);

    connect(itemsPage, &ItemsPage::productsChanged, this, [this]() {
        dashboardPage->refreshDashboard();
        });

    assetsPage = new AssetsPage(assetService, this);
    ui.mainStack->addWidget(assetsPage);

    connect(assetsPage, &AssetsPage::assetsChanged, this, [this]() {
        dashboardPage->refreshDashboard();
        });

    scanInPage = new ScanPage(productService, ScanMode::ScanIn, this);
    ui.mainStack->addWidget(scanInPage);

    connect(scanInPage, &ScanPage::stockChanged, this, [this]() {
        dashboardPage->refreshDashboard();
        itemsPage->refreshProducts();
        });

    scanOutPage = new ScanPage(productService, ScanMode::ScanOut, this);
    ui.mainStack->addWidget(scanOutPage);

    connect(scanOutPage, &ScanPage::stockChanged, this, [this]() {
        dashboardPage->refreshDashboard();
        itemsPage->refreshProducts();
        });

    setupSidebar();
    setupVerticalbar();
	setupDashboardPage();

    ui.mainStack->setCurrentWidget(dashboardPage);
}

// Destructor
DashboardWindow::~DashboardWindow()
{
}

// Initializes the sidebar with navigation buttons and permission-based visibility
void DashboardWindow::setupSidebar()
{
    sidebar = new SidebarWidget(permissions, this);

    QVBoxLayout* layout = new QVBoxLayout(ui.sidebarContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(sidebar);

    // Only Dashboard navigation for now
    connect(sidebar, &SidebarWidget::dashboardClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(dashboardPage);
        });

    connect(sidebar, &SidebarWidget::itemsClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(itemsPage);
        });

    connect(sidebar, &SidebarWidget::assetsClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(assetsPage);
		});



    connect(dashboardPage, &DashboardPage::viewAllItemsRequested, this, [this]() {
        ui.mainStack->setCurrentWidget(itemsPage);
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

// Sets up the top vertical bar displaying user role and name
void DashboardWindow::setupVerticalbar()
{
    verticalbar = new VerticalWidget(role, "Felipe", this);
	QVBoxLayout* layout = new QVBoxLayout(ui.verticalContainer);
    
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(verticalbar);

}

// Initializes the dashboard page content
void DashboardWindow::setupDashboardPage()
{
    ui.mainStack->setCurrentWidget(dashboardPage);

   
    QString perms = "Permissions: ";

    for (const auto& p : permissions) {
        perms += QString::fromStdString(p + " ");
    }

    //ui.permissionsLabel->setText(perms);
}
