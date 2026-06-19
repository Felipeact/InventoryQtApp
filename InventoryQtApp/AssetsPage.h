// AssetsPage.h - Page for managing assets
#pragma once

#include <QWidget>
#include <string>
#include <vector>
#include "ui_AssetsPage.h"
#include "AddAssetDialog.h"
#include "AssetService.h"
#include "Theme.h"

// Page widget that displays and manages assets
class AssetsPage : public QWidget
{
    Q_OBJECT

public:

    void applyTheme(Theme::AppTheme theme);
    // Constructor
    AssetsPage(AssetService& assetService, const std::vector<std::string>& permissions, QWidget* parent = nullptr);
    // Destructor
    ~AssetsPage();

    void setSearchText(const QString& text);


signals:
    void assetsChanged();  // Signal emitted when assets are added/updated/deleted


private slots:
    // Handles the add asset button click
    void onAddAssetClicked();
    void filterAssets(const QString& searchText);  // Filters assets based on search input

    void onFilterButtonClicked();
    void onNextPageClicked();
    void onPrevPageClicked();
    void onPageSizeChanged(int index);

private:
    Ui::AssetsPageClass ui;  // UI components
    AssetService& assetService;  // Reference to asset service for API interactions

    std::vector<std::string> permissions;
    bool hasPermission(const std::string& permission) const;

    json currentAssets;  // Cache for asset data

    QString currentTypeFilter;
    QString currentStatusFilter;

    // Configures the assets table with columns and styling
    void setupTable();
    // Populates the table with asset data
    void loadAssets();
    void populateTable(const json& assets);

    void deleteAsset(const std::string& assetId);  // Deletes an asset by ID

    json filteredAssets;

    int currentPage = 1;
    int pageSize = 10;

    json getCurrentPageAssets() const;
    void updatePagination();


};

