#pragma once

#include <QDialog>
#include "ui_UseTruckItemDialog.h"

class UseTruckItemDialog : public QDialog
{
    Q_OBJECT

public:
    UseTruckItemDialog(QWidget* parent = nullptr);
    ~UseTruckItemDialog();

    void setItem(const QString& itemName, int currentQty);
    int getQuantityToUse() const;
    QString getNotes() const;

private slots:
    void onDecreaseClicked();
    void onIncreaseClicked();
    void onUseItemClicked();
    void onCancelClicked();
    void onItemSelected(int index);

private:
    Ui::UseTruckItemDialogClass ui;
    int maxQuantity;

    void setupConnections();
    void updateQuantityDisplay();
    void loadItems();
};
