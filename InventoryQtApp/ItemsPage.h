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

private slots:
    // Handles the add item button click
    void onAddItemClicked();

private:
    Ui::ItemsPageClass ui;  // UI components
	ProductService& productService;  // Reference to product service for API interactions
    
    // Configures the items table with columns and styling
    void setupTable();
    // Populates the table with product data
    void loadProducts();
};