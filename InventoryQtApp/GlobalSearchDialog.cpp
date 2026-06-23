#include "GlobalSearchDialog.h"

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

    // Behave like a dropdown anchored under the top-bar search box, not a separate
    // window: frameless, no taskbar entry, and shown WITHOUT stealing focus so the
    // user keeps typing in the top bar (which drives this via setSearchText).
    setModal(false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_DeleteOnClose);

    // The top bar is the single search box — hide this dialog's duplicate input and
    // its Open/Close buttons; results open on a single click or Enter.
    ui.searchInput->hide();
    ui.openButton->hide();
    ui.closeButton->hide();

    ui.searchInput->setText(searchText);

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

    // Single click opens a result — it's a dropdown, not a form.
    connect(
        ui.resultsTree,
        &QTreeWidget::itemClicked,
        this,
        [this](QTreeWidgetItem* item, int) {
            if (item && item->childCount() == 0) {
                ui.resultsTree->setCurrentItem(item);
                openSelectedResult();
            }
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

    int totalResults = 0;

    addPageResults(searchText);

    QTreeWidgetItem* pagesGroup =
        ui.resultsTree->topLevelItem(0);

    if (pagesGroup) {
        totalResults += pagesGroup->childCount();
    }

    if (productService) {
        addGroup("Items");

        json products =
            productService->getProducts();

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

        json assets =
            assetService->getAssets();

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

        std::vector<UserDto> users =
            userService->getUsers();

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

        std::vector<TruckDto> trucks =
            truckStockService->getTrucks();

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

        std::vector<StockTemplateDto> templates =
            truckStockService->getTemplates();

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

        std::vector<TruckAssignmentDto> assignments =
            truckStockService->getAssignments();

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

        std::vector<LowStockItemDto> lowStockItems =
            truckStockService->getLowStockItems();

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

        std::vector<ReceiptDto> receipts =
            truckStockService->getReceipts();

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

    // Pre-select the first result so Enter (from the top bar) opens it immediately.
    selectFirstResult();

    ui.summaryLabel->setText(
        QString("%1 result(s) found for \"%2\"")
        .arg(totalResults)
        .arg(searchText)
    );
}

QList<QTreeWidgetItem*> GlobalSearchDialog::selectableItems() const
{
    QList<QTreeWidgetItem*> items;
    for (int i = 0; i < ui.resultsTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* group = ui.resultsTree->topLevelItem(i);
        if (!group) continue;
        for (int j = 0; j < group->childCount(); ++j) {
            QTreeWidgetItem* child = group->child(j);
            if (child && (child->flags() & Qt::ItemIsSelectable)) {
                items.append(child);
            }
        }
    }
    return items;
}

void GlobalSearchDialog::selectFirstResult()
{
    const QList<QTreeWidgetItem*> items = selectableItems();
    if (!items.isEmpty()) {
        ui.resultsTree->setCurrentItem(items.first());
    }
}

void GlobalSearchDialog::moveSelection(int delta)
{
    const QList<QTreeWidgetItem*> items = selectableItems();
    if (items.isEmpty()) return;

    int index = items.indexOf(ui.resultsTree->currentItem());
    if (index < 0) {
        index = (delta >= 0) ? 0 : items.size() - 1;
    } else {
        index += delta;
        if (index < 0) index = 0;
        if (index >= items.size()) index = items.size() - 1;
    }

    QTreeWidgetItem* target = items.at(index);
    ui.resultsTree->setCurrentItem(target);
    ui.resultsTree->scrollToItem(target);
}

void GlobalSearchDialog::activateCurrent()
{
    openSelectedResult();
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