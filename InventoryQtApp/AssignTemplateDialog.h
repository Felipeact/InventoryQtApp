#pragma once

#include <QDialog>
#include <QString>

#include "ui_AssignTemplateDialog.h"
#include "TruckStockService.h"
#include "UserService.h"
#include "Theme.h"

class AssignTemplateDialog : public QDialog
{
    Q_OBJECT

public:

    void applyTheme(Theme::AppTheme theme);
    explicit AssignTemplateDialog(
        TruckStockService* truckStockService,
        UserService* userService,
        QWidget* parent = nullptr
    );

    ~AssignTemplateDialog();

    QString getTruckId() const;
    QString getTemplateId() const;
    QString getTechnicianId() const;
    QString getNotes() const;
    QString getAssignmentDate() const;

private slots:
    void onAssignClicked();
    void onCancelClicked();

private:
    Ui::AssignTemplateDialogClass ui;

    TruckStockService* truckStockService = nullptr;
    UserService* userService = nullptr;

    void setupConnections();

    void loadTrucks();
    void loadTemplates();
    void loadTechnicians();
};