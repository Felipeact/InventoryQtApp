#pragma once

#include <QDialog>
#include <QString>

#include "ui_AssignTemplateDialog.h"
#include "TruckStockService.h"

class AssignTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AssignTemplateDialog(
        TruckStockService* truckStockService,
        QWidget* parent = nullptr
    );

    ~AssignTemplateDialog();

    QString getTruckId() const;
    QString getTemplateId() const;

private slots:
    void onAssignClicked();
    void onCancelClicked();

private:
    Ui::AssignTemplateDialogClass ui;
    TruckStockService* truckStockService = nullptr;

    void setupConnections();
    void loadTrucks();
    void loadTemplates();
};