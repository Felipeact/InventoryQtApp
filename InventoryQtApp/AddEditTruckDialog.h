#pragma once

#include <QDialog>
#include <QString>
#include <QPoint>

#include "ui_AddEditTruckDialog.h"
#include "UserService.h"
#include "Theme.h"

class QMouseEvent;

class AddEditTruckDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEditTruckDialog(
        UserService* userService,
        QWidget* parent = nullptr
    );

    ~AddEditTruckDialog();

    void applyTheme(Theme::AppTheme theme);

    void setEditMode(
        const QString& truckId,
        const QString& truckName,
        const QString& licensePlate,
        const QString& technicianId,
        const QString& status
    );

    QString getTruckName() const;
    QString getLicensePlate() const;
    QString getTechnician() const;
    QString getTechnicianId() const;
    QString getStatus() const;
    QString getNotes() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onCloseClicked();

private:
    Ui::AddEditTruckDialogClass ui;

    UserService* userService = nullptr;
    QString editingTruckId;
    QPoint dragPosition;

    void setupConnections();
    void loadTechnicianList();
};