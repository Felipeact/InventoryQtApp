#pragma once

#include <QDialog>
#include <QPoint>
#include <vector>

#include "ui_TruckStockHistoryDialog.h"
#include "TruckStockService.h"
#include "Theme.h"

class QMouseEvent;

class TruckStockHistoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TruckStockHistoryDialog(
        const MyTruckStockItemDto& item,
        const std::vector<TruckStockMovementDto>& movements,
        QWidget* parent = nullptr
    );

    ~TruckStockHistoryDialog();

    void applyTheme(
        Theme::AppTheme theme
    );

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onCloseClicked();

private:
    Ui::TruckStockHistoryDialogClass ui;

    MyTruckStockItemDto item;
    std::vector<TruckStockMovementDto> movements;

    QPoint dragPosition;

    void setupConnections();
    void setupTable();
    void populateTable();

    QString formatDate(
        const std::string& value
    ) const;
};