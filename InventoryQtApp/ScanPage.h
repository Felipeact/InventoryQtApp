#pragma once

#include <QWidget>

#include "ui_ScanPage.h"
#include "ProductService.h"

enum class ScanMode
{
    ScanIn,
    ScanOut
};

class ScanPage : public QWidget
{
    Q_OBJECT

public:
    ScanPage(
        ProductService& productService,
        ScanMode mode,
        QWidget* parent = nullptr
    );

    ~ScanPage();

signals:
    void stockChanged();

private slots:
    void onSubmitClicked();
    void onMinusClicked();
    void onPlusClicked();

private:
    Ui::ScanPageClass ui;

    ProductService& productService;
    ScanMode mode;

    void setupPage();

    void addRecentScan(
        const QString& barcode,
        const QString& productName,
        int quantity
    );
};