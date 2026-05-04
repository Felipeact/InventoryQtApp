// DashboardWindow.cpp - Implementation of the main dashboard window
#include "DashboardWindow.h"

#include <QVBoxLayout>
#include <QString>
#include <QTableWidgetItem>
#include <QHeaderView>

// Constructor initializes the main window with pages and navigation
DashboardWindow::DashboardWindow(
    const std::string& userRole,
    const std::vector<std::string>& userPermissions,
    ProductService& productService,
    QWidget* parent
)
    : QMainWindow(parent),
    role(userRole),
    permissions(userPermissions),
    productService(&productService)
{
    ui.setupUi(this);

    this->setWindowFlags(
        Qt::Window |
        Qt::WindowMinimizeButtonHint |
        Qt::WindowMaximizeButtonHint |
        Qt::WindowCloseButtonHint
    );

    this->resize(1920, 1080);
    this->setMinimumSize(1000, 700);

	// Add Pages to the main stack
	dashboardPage = new DashboardPage(productService, this);
	ui.mainStack->addWidget(dashboardPage);

    itemsPage = new ItemsPage(productService, this);
	ui.mainStack->addWidget(itemsPage);
    
    setupSidebar();
	setupVerticalbar();    
    setupDashboardPage();


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

    connect(dashboardPage, &DashboardPage::viewAllItemsRequested, this, [this]() {
        ui.mainStack->setCurrentWidget(itemsPage);
        });

    connect(sidebar, &SidebarWidget::logoutClicked, this, [this]() {
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
