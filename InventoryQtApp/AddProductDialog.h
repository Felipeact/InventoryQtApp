#pragma once

#include <QDialog>
#include <QString>

#include "ui_AddProductDialog.h"
#include "Theme.h"

class AddProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddProductDialog(QWidget* parent = nullptr);
    ~AddProductDialog();

    void applyTheme(Theme::AppTheme theme);

    QString getProductName() const;
    QString getBarcode() const;
    int getQuantity() const;

    void setProductData(
        const QString& name,
        const QString& barcode,
        int quantity
    );

    void setViewMode(
        const QString& name,
        const QString& barcode,
        int quantity
    );

private slots:
    void onSaveButtonClicked();
    void onCancelButtonClicked();
    void onCloseButtonClicked();

private:
    Ui::AddProductDialogClass ui;
    bool viewMode = false;
};