#pragma once

#include <QDialog>
#include <QString>

#include "ui_AddEditTruckDialog.h"
#include "UserService.h"

class AddEditTruckDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEditTruckDialog(
        UserService* userService,
        QWidget* parent = nullptr
    );

    ~AddEditTruckDialog();

    void setEditMode(const QString& truckId);

    QString getTruckName() const;
    QString getLicensePlate() const;
    QString getTechnician() const;
    QString getTechnicianId() const;
    QString getStatus() const;
    QString getNotes() const;

private slots:
    void onSaveClicked();
    void onCancelClicked();

private:
    Ui::AddEditTruckDialogClass ui;
    UserService* userService = nullptr;

    void setupConnections();
    void loadTechnicianList();
};