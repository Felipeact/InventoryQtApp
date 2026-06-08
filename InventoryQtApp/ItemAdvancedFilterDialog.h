#pragma once

#include <QDialog>
#include <QPoint>
#include <QString>

#include "ui_ItemAdvancedFilterDialog.h"
#include "Theme.h"

class QMouseEvent;

struct ItemAdvancedFilters
{
    bool enabled = false;

    bool useMinQuantity = false;
    bool useMaxQuantity = false;

    int minQuantity = 0;
    int maxQuantity = 0;

    QString stockStatus = "All";
    QString storeText;
    QString typeText;
};

class ItemAdvancedFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemAdvancedFilterDialog(
        const ItemAdvancedFilters& currentFilters,
        QWidget* parent = nullptr
    );

    ~ItemAdvancedFilterDialog();

    ItemAdvancedFilters getFilters() const;

    void applyTheme(
        Theme::AppTheme theme
    );

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onApplyClicked();
    void onClearClicked();
    void onCancelClicked();
    void onCloseClicked();

private:
    Ui::ItemAdvancedFilterDialogClass ui;

    ItemAdvancedFilters filters;
    QPoint dragPosition;

    void setupConnections();
    void loadFilters();
};