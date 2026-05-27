// ItemsPage.h - Page for managing inventory items
#pragma once

#include <QWidget>
#include "ui_ItemsPage.h"
#include "AddProductDialog.h"
#include "ProductService.h"

// Page widget that displays and manages inventory items
class ItemsPage : public QWidget
{
    Q_OBJECT

public:
    // Constructor
    ItemsPage(ProductService& productService, QWidget* parent = nullptr);
    // Destructor
    ~ItemsPage();

    void refreshProducts();

signals:
	void productsChanged();  // Signal emitted when products are added/updated/deleted
    

private:
    Ui::ItemsPageClass ui;
    ProductService& productService;
    json currentProducts;
    json filteredProducts;

    int currentPage = 1;
    int pageSize = 10;

    void setupTable();
    void loadProducts();
    void populateTable(const json& products);
    void updatePagination();
    json getCurrentPageProducts() const;

    void deleteProduct(const std::string& productId);

private slots:
    void onAddItemClicked();
    void filterProducts(const QString& searchText);
    void onFilterButtonClicked();
    void onNextPageClicked();
    void onPrevPageClicked();
    void onPageSizeChanged(int index);

	
};