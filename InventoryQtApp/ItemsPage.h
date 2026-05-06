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

signals:
	void productsChanged();  // Signal emitted when products are added/updated/deleted
    

private slots:
    // Handles the add item button click
    void onAddItemClicked();
    void filterProducts(const QString& searchText);  // Filters products based on search input

private:
    Ui::ItemsPageClass ui;  // UI components
	ProductService& productService;  // Reference to product service for API interactions
	json currentProducts;  // Cache for product data
    
    // Configures the items table with columns and styling
    void setupTable();
    // Populates the table with product data
    void loadProducts();
    void populateTable(const json& products);

	void deleteProduct(const std::string& productId);  // Deletes a product by ID

	
};