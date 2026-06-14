#pragma once

#include <QDialog>
#include <QString>

#include "ui_GlobalSearchDialog.h"

#include "ProductService.h"
#include "AssetService.h"
#include "UserService.h"
#include "TruckStockService.h"
#include "Theme.h"

#include <vector>

// Snapshot of everything a global search needs, gathered off the GUI thread so
// the dialog never blocks while fetching.
struct GlobalSearchData
{
    json products;
    json assets;
    std::vector<UserDto> users;
    std::vector<TruckDto> trucks;
    std::vector<StockTemplateDto> templates;
    std::vector<TruckAssignmentDto> assignments;
    std::vector<LowStockItemDto> lowStockItems;
    std::vector<ReceiptDto> receipts;
};

class GlobalSearchDialog : public QDialog
{
    Q_OBJECT

public:
    enum class SearchTarget
    {
        Dashboard,
        Items,
        Assets,
        Users,
        Reports,
        Settings,
        TruckDashboard,
        Trucks,
        Templates,
        Assignments,
        MyTruckStock,
        LowStockAlerts,
        Receipts
    };

    explicit GlobalSearchDialog(
        const QString& searchText,
        ProductService* productService,
        AssetService* assetService,
        UserService* userService,
        TruckStockService* truckStockService,
        QWidget* parent = nullptr
    );

    ~GlobalSearchDialog();

    void applyTheme(Theme::AppTheme theme);

    void setSearchText(const QString& text);

signals:
    void resultSelected(
        GlobalSearchDialog::SearchTarget target,
        const QString& searchText
    );

private slots:
    void onSearchChanged(const QString& text);
    void onResultDoubleClicked();
    void onOpenSelectedClicked();
    void onCloseClicked();

private:
    Ui::GlobalSearchDialogClass ui;

    ProductService* productService = nullptr;
    AssetService* assetService = nullptr;
    UserService* userService = nullptr;
    TruckStockService* truckStockService = nullptr;

    bool updatingFromParent = false;

    // Incremented on every search; lets a slow in-flight fetch's callback
    // detect that a newer search has superseded it and bail out.
    int searchGeneration = 0;

    void setupConnections();
    void runSearch(const QString& text);
    void buildResults(const QString& searchText, const GlobalSearchData& data);
    void clearResults();

    void addPageResults(const QString& searchText);
    void addGroup(const QString& title);

    void addResult(
        SearchTarget target,
        const QString& title,
        const QString& subtitle,
        const QString& searchValue
    );

    void openSelectedResult();

    bool containsText(
        const QString& source,
        const QString& searchText
    ) const;
};