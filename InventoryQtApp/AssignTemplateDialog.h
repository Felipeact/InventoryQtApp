#pragma once

#include <QDialog>
#include <QPoint>
#include <QString>

#include "ui_AssignTemplateDialog.h"
#include "TruckStockService.h"
#include "UserService.h"
#include "Theme.h"

class QMouseEvent;

class AssignTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AssignTemplateDialog(
        TruckStockService* truckStockService,
        UserService* userService,
        QWidget* parent = nullptr
    );

    ~AssignTemplateDialog();

    void applyTheme(Theme::AppTheme theme);

    QString getTruckId() const;
    QString getTemplateId() const;
    QString getTechnicianId() const;
    QString getNotes() const;
    QString getAssignmentDate() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onAssignClicked();
    void onCancelClicked();
    void onCloseClicked();

private:
    Ui::AssignTemplateDialogClass ui;

    TruckStockService* truckStockService = nullptr;
    UserService* userService = nullptr;

    QPoint dragPosition;

    void setupConnections();

    void loadTrucks();
    void loadTemplates();
    void loadTechnicians();
};