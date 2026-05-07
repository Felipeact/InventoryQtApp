// AssetsPage.h - Page for managing assets
#pragma once

#include <QWidget>
#include "ui_AssetsPage.h"
#include "AddAssetDialog.h"
#include "AssetService.h"

// Page widget that displays and manages assets
class AssetsPage : public QWidget
{
    Q_OBJECT

public:
    // Constructor
    AssetsPage(AssetService& assetService, QWidget* parent = nullptr);
    // Destructor
    ~AssetsPage();

signals:
    void assetsChanged();  // Signal emitted when assets are added/updated/deleted


private slots:
    // Handles the add asset button click
    void onAddAssetClicked();
    void filterAssets(const QString& searchText);  // Filters assets based on search input

private:
    Ui::AssetsPageClass ui;  // UI components
    AssetService& assetService;  // Reference to asset service for API interactions
    json currentAssets;  // Cache for asset data

    // Configures the assets table with columns and styling
    void setupTable();
    // Populates the table with asset data
    void loadAssets();
    void populateTable(const json& assets);

    void deleteAsset(const std::string& assetId);  // Deletes an asset by ID


};

