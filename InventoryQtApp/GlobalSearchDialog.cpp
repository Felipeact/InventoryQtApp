#include "GlobalSearchDialog.h"
#include "AsyncTask.h"

#include <algorithm>

#include <QLineEdit>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>

GlobalSearchDialog::GlobalSearchDialog(
    const QString& searchText,
    ProductService* productService,
    AssetService* assetService,
    UserService* userService,
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QDialog(parent),
    productService(productService),
    assetService(assetService),
    userService(userService),
    truckStockService(truckStockService)
{
    ui.setupUi(this);

    setModal(false);
    setWindowFlag(Qt::Tool, true);

    ui.searchInput->setText(searchText);
    ui.searchInput->selectAll();

    ui.resultsTree->setColumnCount(2);
    ui.resultsTree->setHeaderLabels(
        QStringList()
        << "Result"
        << "Details"
    );

    setupConnections();
    runSearch(searchText);

    applyTheme(Theme::AppTheme::Dark);
}

GlobalSearchDialog::~GlobalSearchDialog()
{
}

void GlobalSearchDialog::setupConnections()
{
    connect(
        ui.searchInput,
        &QLineEdit::textChanged,
        this,
        &GlobalSearchDialog::onSearchChanged
    );

    connect(
        ui.resultsTree,
        &QTreeWidget::itemDoubleClicked,
        this,
        [this]() {
            onResultDoubleClicked();
        }
    );

    connect(
        ui.openButton,
        &QPushButton::clicked,
        this,
        &GlobalSearchDialog::onOpenSelectedClicked
    );

    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &GlobalSearchDialog::onCloseClicked
    );
}

void GlobalSearchDialog::setSearchText(
    const QString& text
)
{
    if (ui.searchInput->text() == text) {
        runSearch(text);
        return;
    }

    updatingFromParent = true;
    ui.searchInput->setText(text);
    updatingFromParent = false;

    runSearch(text);
}

void GlobalSearchDialog::onSearchChanged(
    const QString& text
)
{
    if (updatingFromParent) {
        return;
    }

    runSearch(text);
}

void GlobalSearchDialog::runSearch(
    const QString& text
)
{
    clearResults();

    QString searchText =
        text.trimmed();

    if (searchText.isEmpty()) {
        ui.summaryLabel->setText("Type something to search.");
        return;
    }

    ui.summaryLabel->setText("Searching…");

    // Service pointers are owned by the parent window and outlive this dialog.
    ProductService* ps = productService;
    AssetService* as = assetService;
    UserService* us = userService;
    TruckStockService* tss = truckStockService;

    const int generation = ++searchGeneration;

    // Gather all data off the GUI thread, then build the tree on the GUI
    // thread. A newer keystroke supersedes this one via the generation guard.
    AsyncTask::run(this,
        [ps, as, us, tss]() {
            GlobalSearchData data;

            if (ps) {
                data.products = ps->getProducts();
            }
            if (as) {
                data.assets = as->getAssets();
            }
            if (us) {
                data.users = us->getUsers();
            }
            if (tss) {
                data.trucks = tss->getTrucks();
                data.templates = tss->getTemplates();
                data.assignments = tss->getAssignments();
                data.lowStockItems = tss->getLowStockItems();
                data.receipts = tss->getReceipts();
            }

            return data;
        },
        [this, searchText, generation](GlobalSearchData data) {
            if (generation != searchGeneration) {
                return; // a newer search has started
            }
            buildResults(searchText, data);
        });
}

void GlobalSearchDialog::buildResults(
    const QString& searchText,
    const GlobalSearchData& data
)
{
    clearResults();

    int totalResults = 0;

    addPageResults(searchText);

    QTreeWidgetItem* pagesGroup =
        ui.resultsTree->topLevelItem(0);

    if (pagesGroup) {
        totalResults += pagesGroup->childCount();
    }

    if (productService) {
        addGroup("Items");

        const json& products = data.products;

        int count = 0;

        for (const auto& product : products) {
            QString name =
                QString::fromStdString(product.value("name", ""));

            QString barcode =
                QString::fromStdString(product.value("barcode", ""));

            QString location =
                QString::fromStdString(product.value("location", ""));

            QString category =
                QString::fromStdString(product.value("category", ""));

            if (
                containsText(name, searchText) ||
                containsText(barcode, searchText) ||
                containsText(location, searchText) ||
                containsText(category, searchText)
                ) {
                addResult(
                    SearchTarget::Items,
                    name.isEmpty() ? "Unnamed item" : name,
                    "Barcode: " + barcode + " | Category: " + category + " | Location: " + location,
                    name
                );

                count++;
                totalResults++;
            }

            if (count >= 8) {
                break;
            }
        }
    }

    if (assetService) {
        addGroup("Assets");

        const json& assets = data.assets;

        int count = 0;

        for (const auto& asset : assets) {
            QString name =
                QString::fromStdString(asset.value("name", ""));

            QString type =
                QString::fromStdString(asset.value("type", ""));

            QString serial =
                QString::fromStdString(asset.value("serialCode", ""));

            QString status =
                QString::fromStdString(asset.value("status", ""));

            if (
                containsText(name, searchText) ||
                containsText(type, searchText) ||
                containsText(serial, searchText) ||
                containsText(status, searchText)
                ) {
                addResult(
                    SearchTarget::Assets,
                    name.isEmpty() ? "Unnamed asset" : name,
                    "Type: " + type + " | Serial: " + serial + " | Status: " + status,
                    name
                );

                count++;
                totalResults++;
            }

            if (count >= 8) {
                break;
            }
        }
    }

    if (userService) {
        addGroup("Users");

        const std::vector<UserDto>& users = data.users;

        int count = 0;

        for (const UserDto& user : users) {
            QString name =
                QString::fromStdString(user.name);

            QString email =
                QString::fromStdString(user.email);

            QString role =
                QString::fromStdString(user.role);

            QString status =
                QString::fromStdString(user.status);

            if (
                containsText(name, searchText) ||
                containsText(email, searchText) ||
                containsText(role, searchText) ||
                containsText(status, searchText)
                ) {
                addResult(
                    SearchTarget::Users,
                    name.isEmpty() ? "Unnamed user" : name,
                    email + " | " + role + " | " + status,
                    name
                );

                count++;
                totalResults++;
            }

            if (count >= 8) {
                break;
            }
        }
    }

    if (truckStockService) {
        addGroup("Trucks");

        const std::vector<TruckDto>& trucks = data.trucks;

        int truckCount = 0;

        for (const TruckDto& truck : trucks) {
            QString truckName =
                QString::fromStdString(truck.truckName);

            QString plate =
                QString::fromStdString(truck.licensePlate);

            QString technician =
                QString::fromStdString(truck.technicianName);

            QString status =
                QString::fromStdString(truck.status);

            if (
                containsText(truckName, searchText) ||
                containsText(plate, searchText) ||
                containsText(technician, searchText) ||
                containsText(status, searchText)
                ) {
                addResult(
                    SearchTarget::Trucks,
                    truckName.isEmpty() ? "Unnamed truck" : truckName,
                    "Plate: " + plate + " | Technician: " + technician + " | Status: " + status,
                    truckName
                );

                truckCount++;
                totalResults++;
            }

            if (truckCount >= 8) {
                break;
            }
        }

        addGroup("Templates");

        const std::vector<StockTemplateDto>& templates = data.templates;

        int templateCount = 0;

        for (const StockTemplateDto& stockTemplate : templates) {
            QString name =
                QString::fromStdString(stockTemplate.name);

            QString trade =
                QString::fromStdString(stockTemplate.tradeType);

            QString items =
                QString::number(stockTemplate.itemCount);

            if (
                containsText(name, searchText) ||
                containsText(trade, searchText) ||
                containsText(items, searchText)
                ) {
                addResult(
                    SearchTarget::Templates,
                    name.isEmpty() ? "Unnamed template" : name,
                    "Trade: " + trade + " | Items: " + items,
                    name
                );

                templateCount++;
                totalResults++;
            }

            if (templateCount >= 8) {
                break;
            }
        }

        addGroup("Assignments");

        const std::vector<TruckAssignmentDto>& assignments = data.assignments;

        int assignmentCount = 0;

        for (const TruckAssignmentDto& assignment : assignments) {
            QString truck =
                QString::fromStdString(assignment.truckNumber);

            QString templateName =
                QString::fromStdString(assignment.templateName);

            QString assignedBy =
                QString::fromStdString(assignment.assignedBy);

            QString status =
                QString::fromStdString(assignment.status);

            if (
                containsText(truck, searchText) ||
                containsText(templateName, searchText) ||
                containsText(assignedBy, searchText) ||
                containsText(status, searchText)
                ) {
                addResult(
                    SearchTarget::Assignments,
                    truck + " - " + templateName,
                    "Assigned by: " + assignedBy + " | Status: " + status,
                    truck
                );

                assignmentCount++;
                totalResults++;
            }

            if (assignmentCount >= 8) {
                break;
            }
        }

        addGroup("Low Stock Alerts");

        const std::vector<LowStockItemDto>& lowStockItems = data.lowStockItems;

        int lowStockCount = 0;

        for (const LowStockItemDto& item : lowStockItems) {
            QString truck =
                QString::fromStdString(item.truckNumber);

            QString product =
                QString::fromStdString(item.productName);

            QString status =
                QString::fromStdString(item.status);

            QString templateName =
                QString::fromStdString(item.templateName);

            if (
                containsText(truck, searchText) ||
                containsText(product, searchText) ||
                containsText(status, searchText) ||
                containsText(templateName, searchText)
                ) {
                addResult(
                    SearchTarget::LowStockAlerts,
                    truck + " - " + product,
                    "Current: " + QString::number(item.currentQuantity) +
                    " | Minimum: " + QString::number(item.minimumQuantity) +
                    " | Status: " + status,
                    product
                );

                lowStockCount++;
                totalResults++;
            }

            if (lowStockCount >= 8) {
                break;
            }
        }

        addGroup("Receipts");

        const std::vector<ReceiptDto>& receipts = data.receipts;

        int receiptCount = 0;

        for (const ReceiptDto& receipt : receipts) {
            QString id =
                QString::fromStdString(receipt.id);

            QString technician =
                QString::fromStdString(receipt.technicianName);

            QString truck =
                QString::fromStdString(receipt.truckNumber);

            QString status =
                QString::fromStdString(receipt.status);

            QString amount =
                "$" + QString::number(receipt.totalAmount, 'f', 2);

            if (
                containsText(id, searchText) ||
                containsText(technician, searchText) ||
                containsText(truck, searchText) ||
                containsText(status, searchText) ||
                containsText(amount, searchText)
                ) {
                addResult(
                    SearchTarget::Receipts,
                    "Receipt " + id,
                    truck + " | " + technician + " | " + amount + " | " + status,
                    id
                );

                receiptCount++;
                totalResults++;
            }

            if (receiptCount >= 8) {
                break;
            }
        }
    }

    ui.resultsTree->expandAll();

    ui.summaryLabel->setText(
        QString("%1 result(s) found for \"%2\"")
        .arg(totalResults)
        .arg(searchText)
    );
}

void GlobalSearchDialog::addPageResults(
    const QString& searchText
)
{
    addGroup("Pages");

    struct PageSearchEntry
    {
        SearchTarget target;
        QString title;
        QString subtitle;
        QString keywords;
    };

    std::vector<PageSearchEntry> pages = {
        {
            SearchTarget::Dashboard,
            "Dashboard",
            "Inventory overview, metrics, recent items, low stock",
            "dashboard home overview metrics inventory"
        },
        {
            SearchTarget::Items,
            "Items",
            "Products, inventory items, barcode, stock quantity",
            "items products inventory barcode stock quantity"
        },
        {
            SearchTarget::Assets,
            "Assets",
            "Company assets, equipment, serial number, status",
            "assets equipment serial status company"
        },
        {
            SearchTarget::Users,
            "Users",
            "Users, admins, technicians, roles and permissions",
            "users user admin technician roles permissions"
        },
        {
            SearchTarget::Reports,
            "Reports",
            "Inventory reports, assets reports, export summaries",
            "reports report export summary inventory assets"
        },
        {
            SearchTarget::Settings,
            "Settings",
            "Profile, appearance, theme, account settings",
            "settings profile appearance theme account"
        },
        {
            SearchTarget::TruckDashboard,
            "Truck Dashboard",
            "Truck stock overview, metrics, recent trucks, low stock",
            "truck dashboard stock overview metrics recent"
        },
        {
            SearchTarget::Trucks,
            "Trucks",
            "Truck list, license plates, technicians, status",
            "trucks truck plate license technician vehicle"
        },
        {
            SearchTarget::Templates,
            "Templates",
            "Stock templates, trade type, required items",
            "templates template stock trade required items"
        },
        {
            SearchTarget::Assignments,
            "Assignments",
            "Assigned templates, trucks, technicians",
            "assignments assignment assigned template truck technician"
        },
        {
            SearchTarget::MyTruckStock,
            "My Truck Stock",
            "Technician truck stock, use item, quantities",
            "my truck stock technician item quantity use"
        },
        {
            SearchTarget::LowStockAlerts,
            "Low Stock Alerts",
            "Critical stock, warning stock, low inventory alerts",
            "low stock alerts critical warning inventory"
        },
        {
            SearchTarget::Receipts,
            "Receipts",
            "Uploaded receipts, approvals, pending receipts",
            "receipts receipt invoice approval pending upload"
        }
    };

    for (const PageSearchEntry& page : pages) {
        if (
            containsText(page.title, searchText) ||
            containsText(page.subtitle, searchText) ||
            containsText(page.keywords, searchText)
            ) {
            addResult(
                page.target,
                page.title,
                page.subtitle,
                searchText
            );
        }
    }
}

void GlobalSearchDialog::clearResults()
{
    ui.resultsTree->clear();
}

void GlobalSearchDialog::addGroup(
    const QString& title
)
{
    QTreeWidgetItem* groupItem =
        new QTreeWidgetItem();

    groupItem->setText(0, title);
    groupItem->setFirstColumnSpanned(true);
    groupItem->setFlags(groupItem->flags() & ~Qt::ItemIsSelectable);

    ui.resultsTree->addTopLevelItem(groupItem);
}

void GlobalSearchDialog::addResult(
    SearchTarget target,
    const QString& title,
    const QString& subtitle,
    const QString& searchValue
)
{
    if (ui.resultsTree->topLevelItemCount() == 0) {
        return;
    }

    QTreeWidgetItem* parent =
        ui.resultsTree->topLevelItem(
            ui.resultsTree->topLevelItemCount() - 1
        );

    if (!parent) {
        return;
    }

    QTreeWidgetItem* item =
        new QTreeWidgetItem(parent);

    item->setText(0, title);
    item->setText(1, subtitle);

    item->setData(
        0,
        Qt::UserRole,
        static_cast<int>(target)
    );

    item->setData(
        0,
        Qt::UserRole + 1,
        searchValue
    );
}

void GlobalSearchDialog::openSelectedResult()
{
    QTreeWidgetItem* item =
        ui.resultsTree->currentItem();

    if (!item || item->childCount() > 0) {
        return;
    }

    SearchTarget target =
        static_cast<SearchTarget>(
            item->data(0, Qt::UserRole).toInt()
            );

    QString searchValue =
        item->data(0, Qt::UserRole + 1).toString();

    emit resultSelected(
        target,
        searchValue
    );

    close();
}

void GlobalSearchDialog::onResultDoubleClicked()
{
    openSelectedResult();
}

void GlobalSearchDialog::onOpenSelectedClicked()
{
    openSelectedResult();
}

void GlobalSearchDialog::onCloseClicked()
{
    close();
}

bool GlobalSearchDialog::containsText(
    const QString& source,
    const QString& searchText
) const
{
    return source.contains(
        searchText,
        Qt::CaseInsensitive
    );
}

void GlobalSearchDialog::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::notificationDialogStyle(theme)
    );
}