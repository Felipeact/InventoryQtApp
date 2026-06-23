#pragma once

#include <QDialog>
#include <QString>
#include <QList>

#include "ui_GlobalSearchDialog.h"

#include "ProductService.h"
#include "AssetService.h"
#include "UserService.h"
#include "TruckStockService.h"
#include "Theme.h"

class QTreeWidgetItem;

class GlobalSearchDialog : public QDialog
{
    Q_OBJECT

public:
    enum class SearchTarget
    {
        Dashboard,
        Items,
        Assets,
        Users,
        Reports,
        Settings,
        TruckDashboard,
        Trucks,
        Templates,
        Assignments,
        MyTruckStock,
        LowStockAlerts,
        Receipts
    };

    explicit GlobalSearchDialog(
        const QString& searchText,
        ProductService* productService,
        AssetService* assetService,
        UserService* userService,
        TruckStockService* truckStockService,
        QWidget* parent = nullptr
    );

    ~GlobalSearchDialog();

    void applyTheme(Theme::AppTheme theme);

    void setSearchText(const QString& text);

    /** Keyboard navigation driven from the top-bar search box (focus stays there). */
    void moveSelection(int delta);
    void activateCurrent();

signals:
    void resultSelected(
        GlobalSearchDialog::SearchTarget target,
        const QString& searchText
    );

private slots:
    void onSearchChanged(const QString& text);
    void onResultDoubleClicked();
    void onOpenSelectedClicked();
    void onCloseClicked();

private:
    Ui::GlobalSearchDialogClass ui;

    ProductService* productService = nullptr;
    AssetService* assetService = nullptr;
    UserService* userService = nullptr;
    TruckStockService* truckStockService = nullptr;

    bool updatingFromParent = false;

    void setupConnections();
    void runSearch(const QString& text);
    void clearResults();

    void addPageResults(const QString& searchText);
    void addGroup(const QString& title);

    void addResult(
        SearchTarget target,
        const QString& title,
        const QString& subtitle,
        const QString& searchValue
    );

    void openSelectedResult();

    /** All selectable leaf results, in display order (for keyboard navigation). */
    QList<QTreeWidgetItem*> selectableItems() const;
    void selectFirstResult();

    bool containsText(
        const QString& source,
        const QString& searchText
    ) const;
};