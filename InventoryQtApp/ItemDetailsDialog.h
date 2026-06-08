#pragma once

#include <QDialog>
#include <QPoint>

#include "ui_ItemDetailsDialog.h"
#include "ProductService.h"
#include "Theme.h"

class QMouseEvent;

class ItemDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemDetailsDialog(
        const json& product,
        QWidget* parent = nullptr
    );

    ~ItemDetailsDialog();

    void applyTheme(
        Theme::AppTheme theme
    );

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onCloseClicked();

private:
    Ui::ItemDetailsDialogClass ui;

    json product;
    QPoint dragPosition;

    void setupConnections();
    void loadProductDetails();
    void loadImagePreview();

    QString getStringValue(
        const std::string& key,
        const QString& fallback = "-"
    ) const;

    int getQuantity() const;
    QString getImageValue() const;
};