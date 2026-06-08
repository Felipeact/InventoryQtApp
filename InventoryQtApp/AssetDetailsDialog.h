#pragma once

#include <QDialog>
#include <QPoint>

#include "ui_AssetDetailsDialog.h"
#include "AssetService.h"
#include "Theme.h"

class QMouseEvent;

class AssetDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AssetDetailsDialog(
        const json& asset,
        QWidget* parent = nullptr
    );

    ~AssetDetailsDialog();

    void applyTheme(
        Theme::AppTheme theme
    );

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onCloseClicked();

private:
    Ui::AssetDetailsDialogClass ui;

    json asset;
    QPoint dragPosition;

    void setupConnections();
    void loadAssetDetails();

    QString getStringValue(
        const std::string& key,
        const QString& fallback = "-"
    ) const;

    QString getDateOnly(
        const QString& value
    ) const;
};