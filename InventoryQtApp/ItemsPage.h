#pragma once

#include <QWidget>
#include <string>
#include <vector>

#include "ui_ItemsPage.h"
#include "AddProductDialog.h"
#include "ProductService.h"
#include "Theme.h"
#include "ItemAdvancedFilterDialog.h"

class ItemsPage : public QWidget
{
    Q_OBJECT

public:
    ItemsPage(
        ProductService& productService,
        const std::vector<std::string>& permissions,
        QWidget* parent = nullptr
    );

    ~ItemsPage();

    void refreshProducts();
    void setSearchText(const QString& text);

    void applyTheme(
        Theme::AppTheme theme
    );

signals:
    void productsChanged();

private:
    Ui::ItemsPageClass ui;

    ProductService& productService;

    std::vector<std::string> permissions;
    bool hasPermission(const std::string& permission) const;

    json currentProducts;
    json filteredProducts;

    ItemAdvancedFilters advancedFilters;

    int currentPage = 1;
    int pageSize = 10;

    void setupConnections();
    void setupTable();

    void loadProducts();
    void populateTable(
        const json& products
    );

    void updatePagination();

    json getCurrentPageProducts() const;

    void deleteProduct(
        const std::string& productId
    );

    void applyFilters();

    bool productMatchesSearch(
        const json& product,
        const QString& searchText
    ) const;

    bool productMatchesAdvancedFilters(
        const json& product
    ) const;

    int getProductQuantity(
        const json& product
    ) const;

    int getProductMinimumQuantity(
        const json& product
    ) const;

    QString getProductString(
        const json& product,
        const std::string& key,
        const QString& fallback = ""
    ) const;

    QString getProductImageValue(
        const json& product
    ) const;

    void setImageCell(
        int row,
        int column,
        const QString& imagePathOrUrl
    );

private slots:
    void onAddItemClicked();
    void filterProducts(const QString& searchText);
    void onFilterButtonClicked();

    void onNextPageClicked();
    void onPrevPageClicked();
    void onPage2Clicked();
    void onPageSizeChanged(int index);
};