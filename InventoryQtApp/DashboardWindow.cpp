#include "DashboardWindow.h"

#include <QVBoxLayout>
#include <QString>
#include <QTableWidgetItem>
#include <QHeaderView>

DashboardWindow::DashboardWindow(
    const std::string& userRole,
    const std::vector<std::string>& userPermissions,
    QWidget* parent
)
    : QMainWindow(parent),
    role(userRole),
    permissions(userPermissions)
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
	dashboardPage = new DashboardPage(this);
	ui.mainStack->addWidget(dashboardPage);

    itemsPage = new ItemsPage(this);;
	ui.mainStack->addWidget(itemsPage);
    
    setupSidebar();
	setupVerticalbar();    
    setupDashboardPage();


}

DashboardWindow::~DashboardWindow()
{
}

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

    connect(sidebar, &SidebarWidget::logoutClicked, this, [this]() {
        this->close();
		});

   
}

void DashboardWindow::setupVerticalbar()
{
    verticalbar = new VerticalWidget(role, "Felipe", this);
	QVBoxLayout* layout = new QVBoxLayout(ui.verticalContainer);
    
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(verticalbar);
   
}

void DashboardWindow::setupDashboardPage()
{
    ui.mainStack->setCurrentWidget(dashboardPage);

   
    QString perms = "Permissions: ";

    for (const auto& p : permissions) {
        perms += QString::fromStdString(p + " ");
    }

    //ui.permissionsLabel->setText(perms);
}
