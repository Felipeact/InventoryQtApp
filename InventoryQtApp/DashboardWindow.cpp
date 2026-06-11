// DashboardWindow.cpp - Implementation of the main dashboard window
#include "DashboardWindow.h"
#include "Theme.h"

#include <QVBoxLayout>
#include <QString>
#include <QSettings>
#include <QPoint>

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
    reportService(&reportService),
    truckStockService(&truckStockService)
{
    ui.setupUi(this);

    setWindowTitle("Inventory Dashboard");
    resize(1440, 900);
    setMinimumSize(1100, 720);

    loadSavedTheme();
    setupPages();
    setupSidebar();
    setupVerticalbar();
    applyThemeToLoadedPages();

    ui.mainStack->setCurrentWidget(ensureDashboardPage());
}

DashboardWindow::~DashboardWindow()
{
}

void DashboardWindow::loadSavedTheme()
{
    QSettings settings("InventorySystem", "InventoryQtApp");
    QString savedTheme = settings.value("appearance/theme", "Dark").toString();
    currentTheme = savedTheme == "Light" ? Theme::AppTheme::Light : Theme::AppTheme::Dark;
    Theme::applyTheme(currentTheme);
    applyTheme(currentTheme);
}

void DashboardWindow::applyThemeToLoadedPages()
{
    applyTheme(currentTheme);
    if (dashboardPage) dashboardPage->applyTheme(currentTheme);
    if (itemsPage) itemsPage->applyTheme(currentTheme);
    if (assetsPage) assetsPage->applyTheme(currentTheme);
    if (usersPage) usersPage->applyTheme(currentTheme);
    if (settingsPage) settingsPage->applyTheme(currentTheme);
    if (sidebar) sidebar->applyTheme(currentTheme);
    if (verticalbar) verticalbar->applyTheme(currentTheme);
    if (scanInPage) scanInPage->applyTheme(currentTheme);
    if (scanOutPage) scanOutPage->applyTheme(currentTheme);
    if (truckStockDashboardPage) truckStockDashboardPage->applyTheme(currentTheme);
    if (trucksPage) trucksPage->applyTheme(currentTheme);
    if (stockTemplatesPage) stockTemplatesPage->applyTheme(currentTheme);
    if (assignmentsPage) assignmentsPage->applyTheme(currentTheme);
    if (myTruckStockPage) myTruckStockPage->applyTheme(currentTheme);
    if (lowStockAlertsPage) lowStockAlertsPage->applyTheme(currentTheme);
    if (receiptsPage) receiptsPage->applyTheme(currentTheme);
    if (reportsPage) reportsPage->applyTheme(currentTheme);
}

void DashboardWindow::setupPages()
{
    ensureDashboardPage();
}

DashboardPage* DashboardWindow::ensureDashboardPage()
{
    if (!dashboardPage) {
        dashboardPage = new DashboardPage(*productService, *reportService, this);
        ui.mainStack->addWidget(dashboardPage);
        dashboardPage->applyTheme(currentTheme);

        connect(
            dashboardPage,
            &DashboardPage::viewAllItemsRequested,
            this,
            [this]() {
                ItemsPage* page =
                    ensureItemsPage();

                page->refreshProducts();
                ui.mainStack->setCurrentWidget(page);

                if (sidebar) {
                    sidebar->activateItems();
                }
            }
        );

        connect(
            dashboardPage,
            &DashboardPage::viewAllLowStockRequested,
            this,
            [this]() {
                LowStockAlertsPage* page =
                    ensureLowStockAlertsPage();

                page->refreshAlerts();
                ui.mainStack->setCurrentWidget(page);

                if (sidebar) {
                    sidebar->activateLowStockAlerts();
                }
            }
        );
    }
    return dashboardPage;
}

ItemsPage* DashboardWindow::ensureItemsPage()
{
    if (!itemsPage) {
        itemsPage = new ItemsPage(*productService, this);
        ui.mainStack->addWidget(itemsPage);
        itemsPage->applyTheme(currentTheme);
        connect(itemsPage, &ItemsPage::productsChanged, this, [this]() {
            if (dashboardPage) dashboardPage->refreshDashboard();
            });
    }
    return itemsPage;
}

AssetsPage* DashboardWindow::ensureAssetsPage()
{
    if (!assetsPage) {
        assetsPage = new AssetsPage(*assetService, this);
        ui.mainStack->addWidget(assetsPage);
        assetsPage->applyTheme(currentTheme);
        connect(assetsPage, &AssetsPage::assetsChanged, this, [this]() {
            if (dashboardPage) dashboardPage->refreshDashboard();
            });
    }
    return assetsPage;
}

UsersPage* DashboardWindow::ensureUsersPage()
{
    if (!usersPage) {
        usersPage = new UsersPage(userService, this);
        usersPage->setLoggedInUserName(userName);
        ui.mainStack->addWidget(usersPage);
        usersPage->applyTheme(currentTheme);
        connect(usersPage, &UsersPage::loggedInUserUpdated, this, &DashboardWindow::updateLoggedInUserInfo);
    }
    return usersPage;
}

ScanPage* DashboardWindow::ensureScanInPage()
{
    if (!scanInPage) {
        scanInPage = new ScanPage(*productService, ScanMode::ScanIn, this);
        ui.mainStack->addWidget(scanInPage);
        scanInPage->applyTheme(currentTheme);
        connect(scanInPage, &ScanPage::stockChanged, this, [this]() {
            if (dashboardPage) dashboardPage->refreshDashboard();
            if (itemsPage) itemsPage->refreshProducts();
            });
    }
    return scanInPage;
}

ScanPage* DashboardWindow::ensureScanOutPage()
{
    if (!scanOutPage) {
        scanOutPage = new ScanPage(*productService, ScanMode::ScanOut, this);
        ui.mainStack->addWidget(scanOutPage);
        scanOutPage->applyTheme(currentTheme);
        connect(scanOutPage, &ScanPage::stockChanged, this, [this]() {
            if (dashboardPage) dashboardPage->refreshDashboard();
            if (itemsPage) itemsPage->refreshProducts();
            });
    }
    return scanOutPage;
}

SettingsPage* DashboardWindow::ensureSettingsPage()
{
    if (!settingsPage) {
        settingsPage = new SettingsPage(role, userName, userService, this);
        ui.mainStack->addWidget(settingsPage);
        settingsPage->applyTheme(currentTheme);

        connect(settingsPage, &SettingsPage::userNameChanged, this, &DashboardWindow::updateLoggedInUserInfo);

        connect(settingsPage, &SettingsPage::themeChanged, this, [this](const QString& themeName) {
            QSettings settings("InventorySystem", "InventoryQtApp");
            settings.setValue("appearance/theme", themeName);
            currentTheme = themeName == "Light" ? Theme::AppTheme::Light : Theme::AppTheme::Dark;
            Theme::applyTheme(currentTheme);
            applyThemeToLoadedPages();
            });

        connect(settingsPage, &SettingsPage::logoutRequested, this, [this]() {
            emit logoutRequested();
            close();
            });
    }
    return settingsPage;
}

TruckStockDashboardPage* DashboardWindow::ensureTruckStockDashboardPage()
{
    if (!truckStockDashboardPage) {
        truckStockDashboardPage = new TruckStockDashboardPage(truckStockService, this);
        ui.mainStack->addWidget(truckStockDashboardPage);
        truckStockDashboardPage->applyTheme(currentTheme);

        connect(truckStockDashboardPage, &TruckStockDashboardPage::viewAllTrucksRequested, this, [this]() {
            TrucksPage* page = ensureTrucksPage();
            page->refreshTrucksList();
            ui.mainStack->setCurrentWidget(page);

            if (sidebar) {
                sidebar->activateTrucks();
            }
            });

        connect(truckStockDashboardPage, &TruckStockDashboardPage::viewAllLowStockRequested, this, [this]() {
            LowStockAlertsPage* page = ensureLowStockAlertsPage();
            page->refreshAlerts();
            ui.mainStack->setCurrentWidget(page);

            if (sidebar) {
                sidebar->activateLowStockAlerts();
            }
            });
    }
    return truckStockDashboardPage;
}

TrucksPage* DashboardWindow::ensureTrucksPage()
{
    if (!trucksPage) {
        trucksPage = new TrucksPage(truckStockService, userService, this);
        ui.mainStack->addWidget(trucksPage);
        trucksPage->applyTheme(currentTheme);
        connect(trucksPage, &TrucksPage::trucksChanged, this, [this]() {
            if (truckStockDashboardPage) truckStockDashboardPage->refreshDashboard();
            });
    }
    return trucksPage;
}

StockTemplatesPage* DashboardWindow::ensureStockTemplatesPage()
{
    if (!stockTemplatesPage) {
        stockTemplatesPage = new StockTemplatesPage(truckStockService, this);
        ui.mainStack->addWidget(stockTemplatesPage);
        stockTemplatesPage->applyTheme(currentTheme);
    }
    return stockTemplatesPage;
}

AssignmentsPage* DashboardWindow::ensureAssignmentsPage()
{
    if (!assignmentsPage) {
        assignmentsPage = new AssignmentsPage(truckStockService, userService, this);
        ui.mainStack->addWidget(assignmentsPage);
        assignmentsPage->applyTheme(currentTheme);
        connect(assignmentsPage, &AssignmentsPage::assignmentsChanged, this, [this]() {
            if (truckStockDashboardPage) truckStockDashboardPage->refreshDashboard();
            if (myTruckStockPage) myTruckStockPage->refreshStock();
            if (lowStockAlertsPage) lowStockAlertsPage->refreshAlerts();
            });
    }
    return assignmentsPage;
}

MyTruckStockPage* DashboardWindow::ensureMyTruckStockPage()
{
    if (!myTruckStockPage) {
        myTruckStockPage = new MyTruckStockPage(truckStockService, this);
        ui.mainStack->addWidget(myTruckStockPage);
        myTruckStockPage->applyTheme(currentTheme);
        connect(myTruckStockPage, &MyTruckStockPage::stockChanged, this, [this]() {
            if (truckStockDashboardPage) truckStockDashboardPage->refreshDashboard();
            if (lowStockAlertsPage) lowStockAlertsPage->refreshAlerts();
            });
    }
    return myTruckStockPage;
}

LowStockAlertsPage* DashboardWindow::ensureLowStockAlertsPage()
{
    if (!lowStockAlertsPage) {
        lowStockAlertsPage = new LowStockAlertsPage(truckStockService, this);
        ui.mainStack->addWidget(lowStockAlertsPage);
        lowStockAlertsPage->applyTheme(currentTheme);
    }
    return lowStockAlertsPage;
}

ReceiptsPage* DashboardWindow::ensureReceiptsPage()
{
    if (!receiptsPage) {
        receiptsPage = new ReceiptsPage(truckStockService, permissions, this);
        ui.mainStack->addWidget(receiptsPage);
        receiptsPage->applyTheme(currentTheme);
        connect(receiptsPage, &ReceiptsPage::receiptsChanged, this, [this]() {
            if (truckStockDashboardPage) truckStockDashboardPage->refreshDashboard();
            if (lowStockAlertsPage) lowStockAlertsPage->refreshAlerts();
            if (myTruckStockPage) myTruckStockPage->refreshStock();
            });
    }
    return receiptsPage;
}

ReportsPage* DashboardWindow::ensureReportsPage()
{
    if (!reportsPage) {
        reportsPage = new ReportsPage(reportService, truckStockService, this);
        ui.mainStack->addWidget(reportsPage);
        reportsPage->applyTheme(currentTheme);
    }
    return reportsPage;
}

void DashboardWindow::updateLoggedInUserInfo(const std::string& newUserName)
{
    userName = newUserName;

    if (verticalbar) {
        verticalbar->setUserInfo(role, userName);
    }

    if (sidebar) {
        sidebar->setUserInfo(role, userName);
    }

    if (usersPage) {
        usersPage->setLoggedInUserName(userName);
        usersPage->refreshUsers();
    }

    if (settingsPage) {
        settingsPage->setUserInfo(role, userName);
    }
}

void DashboardWindow::setupSidebar()
{
    sidebar = new SidebarWidget(permissions, role, userName, this);

    QVBoxLayout* layout = new QVBoxLayout(ui.sidebarContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(sidebar);

    connect(sidebar, &SidebarWidget::dashboardClicked, this, [this]() {
        DashboardPage* page = ensureDashboardPage();
        page->refreshDashboard();
        ui.mainStack->setCurrentWidget(page);
        });

    connect(sidebar, &SidebarWidget::itemsClicked, this, [this]() {
        ItemsPage* page = ensureItemsPage();
        page->refreshProducts();
        ui.mainStack->setCurrentWidget(page);
        });

    connect(sidebar, &SidebarWidget::assetsClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(ensureAssetsPage());
        });

    connect(sidebar, &SidebarWidget::usersClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(ensureUsersPage());
        });

    connect(sidebar, &SidebarWidget::scanInClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(ensureScanInPage());
        });

    connect(sidebar, &SidebarWidget::scanOutClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(ensureScanOutPage());
        });

    connect(sidebar, &SidebarWidget::settingsClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(ensureSettingsPage());
        });

    connect(sidebar, &SidebarWidget::truckStockDashboardClicked, this, [this]() {
        TruckStockDashboardPage* page = ensureTruckStockDashboardPage();
        page->refreshDashboard();
        ui.mainStack->setCurrentWidget(page);
        });

    connect(sidebar, &SidebarWidget::trucksClicked, this, [this]() {
        TrucksPage* page = ensureTrucksPage();
        page->refreshTrucksList();
        ui.mainStack->setCurrentWidget(page);
        });

    connect(sidebar, &SidebarWidget::templatesClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(ensureStockTemplatesPage());
        });

    connect(sidebar, &SidebarWidget::assignmentsClicked, this, [this]() {
        ui.mainStack->setCurrentWidget(ensureAssignmentsPage());
        });

    connect(sidebar, &SidebarWidget::myTruckStockClicked, this, [this]() {
        MyTruckStockPage* page = ensureMyTruckStockPage();
        page->refreshStock();
        ui.mainStack->setCurrentWidget(page);
        });

    connect(sidebar, &SidebarWidget::lowStockAlertsClicked, this, [this]() {
        LowStockAlertsPage* page = ensureLowStockAlertsPage();
        page->refreshAlerts();
        ui.mainStack->setCurrentWidget(page);

        if (sidebar) {
            sidebar->activateLowStockAlerts();
        }
        });

    connect(sidebar, &SidebarWidget::receiptsClicked, this, [this]() {
        ReceiptsPage* page = ensureReceiptsPage();
        page->refreshReceipts();
        ui.mainStack->setCurrentWidget(page);

        if (sidebar) {
            sidebar->activateReceipts();
        }
        });

    connect(sidebar, &SidebarWidget::reportsClicked, this, [this]() {
        ReportsPage* page = ensureReportsPage();
        page->refreshReports();
        ui.mainStack->setCurrentWidget(page);
        });

    connect(sidebar, &SidebarWidget::logoutClicked, this, [this]() {
        emit logoutRequested();
        close();
        });

    sidebar->applyTheme(currentTheme);
}

void DashboardWindow::setupVerticalbar()
{
    verticalbar = new VerticalWidget(role, userName, this);

    connect(verticalbar, &VerticalWidget::globalSearchTextChanged, this, &DashboardWindow::onGlobalSearchTextChanged);
    connect(verticalbar, &VerticalWidget::notificationRequested, this, &DashboardWindow::onNotificationRequested);

    QVBoxLayout* layout = new QVBoxLayout(ui.verticalContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(verticalbar);

    verticalbar->applyTheme(currentTheme);
}

void DashboardWindow::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(Theme::shellStyle(theme));
}

void DashboardWindow::onGlobalSearchTextChanged(const QString& text)
{
    QString searchText = text.trimmed();

    if (searchText.length() < 2) {
        closeGlobalSearchDialog();
        return;
    }

    showGlobalSearchDialog(searchText);
}

void DashboardWindow::showGlobalSearchDialog(const QString& text)
{
    if (!globalSearchDialog) {
        globalSearchDialog = new GlobalSearchDialog(text, productService, assetService, userService, truckStockService, this);

        connect(globalSearchDialog, &GlobalSearchDialog::resultSelected, this, &DashboardWindow::handleGlobalSearchResult);
        connect(globalSearchDialog, &QObject::destroyed, this, [this]() { globalSearchDialog = nullptr; });

        QPoint globalPos = ui.verticalContainer->mapToGlobal(QPoint(0, ui.verticalContainer->height()));
        globalSearchDialog->move(globalPos.x() + 260, globalPos.y() + 6);
        globalSearchDialog->show();
    }

    globalSearchDialog->setSearchText(text);
    globalSearchDialog->raise();
    globalSearchDialog->activateWindow();
}

void DashboardWindow::closeGlobalSearchDialog()
{
    if (globalSearchDialog) {
        globalSearchDialog->close();
        globalSearchDialog = nullptr;
    }
}

void DashboardWindow::handleGlobalSearchResult(GlobalSearchDialog::SearchTarget target, const QString& value)
{
    closeGlobalSearchDialog();
    if (verticalbar) verticalbar->clearSearch();

    switch (target) {
    case GlobalSearchDialog::SearchTarget::Dashboard:
        ensureDashboardPage()->refreshDashboard();
        ui.mainStack->setCurrentWidget(dashboardPage);
        if (sidebar) sidebar->activateDashboard();
        break;
    case GlobalSearchDialog::SearchTarget::Items:
        ensureItemsPage()->setSearchText(value);
        ui.mainStack->setCurrentWidget(itemsPage);
        if (sidebar) sidebar->activateItems();
        break;
    case GlobalSearchDialog::SearchTarget::Assets:
        ensureAssetsPage()->setSearchText(value);
        ui.mainStack->setCurrentWidget(assetsPage);
        if (sidebar) sidebar->activateAssets();
        break;
    case GlobalSearchDialog::SearchTarget::Users:
        ensureUsersPage()->setSearchText(value);
        ui.mainStack->setCurrentWidget(usersPage);
        if (sidebar) sidebar->activateUsers();
        break;
    case GlobalSearchDialog::SearchTarget::Reports:
        ensureReportsPage()->refreshReports();
        ui.mainStack->setCurrentWidget(reportsPage);
        if (sidebar) sidebar->activateReports();
        break;
    case GlobalSearchDialog::SearchTarget::Settings:
        ui.mainStack->setCurrentWidget(ensureSettingsPage());
        if (sidebar) sidebar->activateSettings();
        break;
    case GlobalSearchDialog::SearchTarget::TruckDashboard:
        ensureTruckStockDashboardPage()->refreshDashboard();
        ui.mainStack->setCurrentWidget(truckStockDashboardPage);
        if (sidebar) sidebar->activateTruckStockDashboard();
        break;
    case GlobalSearchDialog::SearchTarget::Trucks:
        ensureTrucksPage()->setSearchText(value);
        ui.mainStack->setCurrentWidget(trucksPage);
        if (sidebar) sidebar->activateTrucks();
        break;
    case GlobalSearchDialog::SearchTarget::Templates:
        ensureStockTemplatesPage()->setSearchText(value);
        ui.mainStack->setCurrentWidget(stockTemplatesPage);
        if (sidebar) sidebar->activateTemplates();
        break;
    case GlobalSearchDialog::SearchTarget::Assignments:
        ensureAssignmentsPage()->setSearchText(value);
        ui.mainStack->setCurrentWidget(assignmentsPage);
        if (sidebar) sidebar->activateAssignments();
        break;
    case GlobalSearchDialog::SearchTarget::MyTruckStock:
        ensureMyTruckStockPage()->setSearchText(value);
        ui.mainStack->setCurrentWidget(myTruckStockPage);
        if (sidebar) sidebar->activateMyTruckStock();
        break;
    case GlobalSearchDialog::SearchTarget::LowStockAlerts:
        ensureLowStockAlertsPage()->setSearchText(value);
        ui.mainStack->setCurrentWidget(lowStockAlertsPage);
        if (sidebar) sidebar->activateLowStockAlerts();
        break;
    case GlobalSearchDialog::SearchTarget::Receipts:
        ensureReceiptsPage()->setSearchText(value);
        ui.mainStack->setCurrentWidget(receiptsPage);
        if (sidebar) sidebar->activateReceipts();
        break;
    }
}

void DashboardWindow::onNotificationRequested()
{
    NotificationDialog dialog(truckStockService, this);

    connect(&dialog, &NotificationDialog::openLowStockRequested, this, [this]() {
        LowStockAlertsPage* page = ensureLowStockAlertsPage();
        page->refreshAlerts();
        ui.mainStack->setCurrentWidget(page);
        });

    connect(&dialog, &NotificationDialog::openReceiptsRequested, this, [this]() {
        ReceiptsPage* page = ensureReceiptsPage();
        page->refreshReceipts();
        ui.mainStack->setCurrentWidget(page);

        if (sidebar) {
            sidebar->activateReceipts();
        }
        });

    dialog.exec();
}
