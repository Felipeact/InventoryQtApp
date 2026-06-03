#pragma once

#include <QDialog>
#include <QPoint>
#include <QString>

#include "ui_UseTruckItemDialog.h"
#include "Theme.h"

class QMouseEvent;

class UseTruckItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UseTruckItemDialog(QWidget* parent = nullptr);
    ~UseTruckItemDialog();

    void applyTheme(Theme::AppTheme theme);

    void setItem(
        const QString& itemName,
        int currentQty
    );

    int getQuantityToUse() const;
    QString getNotes() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onDecreaseClicked();
    void onIncreaseClicked();
    void onUseItemClicked();
    void onCancelClicked();
    void onCloseClicked();

private:
    Ui::UseTruckItemDialogClass ui;

    int maxQuantity = 0;
    QPoint dragPosition;

    void setupConnections();
};