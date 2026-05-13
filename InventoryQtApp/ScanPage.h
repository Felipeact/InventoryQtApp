// ScanPage.h - Page for scan in/out operations
#pragma once

#include <QWidget>
#include "ui_ScanPage.h"
#include "ProductService.h"

enum class ScanMode
{
    ScanIn,   // Mode for scanning items into inventory
    ScanOut   // Mode for scanning items out of inventory
};

// Widget for scanning items with barcode input and quantity selection
class ScanPage : public QWidget
{
    Q_OBJECT

public:
    ScanPage(ProductService& productService, ScanMode mode, QWidget* parent = nullptr);
    ~ScanPage();

signals:
    void stockChanged();

private slots:
    void onSubmitClicked();

private:
    Ui::ScanPageClass ui;
    ProductService& productService;
    ScanMode mode;

    void setupPage();
};