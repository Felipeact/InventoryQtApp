#pragma once

#include <QDialog>
#include <QString>

#include "ui_AddAssetDialog.h"
#include "Theme.h"

class AddAssetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddAssetDialog(QWidget* parent = nullptr);
    ~AddAssetDialog();

    void applyTheme(Theme::AppTheme theme);

    QString getAssetName() const;
    QString getAssetType() const;
    QString getSerialCode() const;
    QString getStatus() const;
    QString getDescription() const;

    void setAssetData(
        const QString& name,
        const QString& type,
        const QString& serialCode,
        const QString& status,
        const QString& description = ""
    );

    void setViewMode(
        const QString& name,
        const QString& type,
        const QString& serialCode,
        const QString& status,
        const QString& description = ""
    );

private slots:
    void onSaveButtonClicked();
    void onCancelButtonClicked();
    void onCloseButtonClicked();

private:
    Ui::AddAssetDialogClass ui;
    bool viewMode = false;
};