// AddAssetDialog.h - Dialog for adding and editing assets
#pragma once

#include <QDialog>
#include "ui_AddAssetDialog.h"
#include "Theme.h"

// Dialog window for collecting asset information when adding or editing items
class AddAssetDialog : public QDialog
{
    Q_OBJECT

public:

    // Constructor
    AddAssetDialog(QWidget* parent = nullptr);
    // Destructor
    ~AddAssetDialog();

    // Getters for asset information
    QString getAssetName() const;        // Returns trimmed asset name
    QString getAssetType() const;       // Returns trimmed asset type
    QString getSerialCode() const;      // Returns trimmed serial code
    QString getStatus() const;          // Returns selected status
    QString getDescription() const;     // Returns trimmed description

    void applyTheme(Theme::AppTheme theme);
    void setAssetData(const QString& name, const QString& type, const QString& serialCode, const QString& status, const QString& description = "");
    void setViewMode(const QString& name, const QString& type, const QString& serialCode, const QString& status, const QString& description = "");

private slots:
    // Slot handlers for button clicks
    void onSaveButtonClicked();   // Validates input and accepts dialog
    void onCancelButtonClicked(); // Rejects dialog without saving
    void onCloseButtonClicked();  // Closes dialog

private:
    Ui::AddAssetDialogClass ui;


};

