#pragma once

#include <QDialog>
#include <QString>

#include "ui_UseTruckItemDialog.h"

class UseTruckItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UseTruckItemDialog(QWidget* parent = nullptr);
    ~UseTruckItemDialog();

    void setItem(
        const QString& itemName,
        int currentQty
    );

    int getQuantityToUse() const;
    QString getNotes() const;

private slots:
    void onDecreaseClicked();
    void onIncreaseClicked();
    void onUseItemClicked();
    void onCancelClicked();

private:
    Ui::UseTruckItemDialogClass ui;

    int maxQuantity = 0;

    void setupConnections();
};