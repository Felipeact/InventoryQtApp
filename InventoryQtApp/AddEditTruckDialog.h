#pragma once

#include <QDialog>
#include "ui_AddEditTruckDialog.h"

class AddEditTruckDialog : public QDialog
{
    Q_OBJECT

public:
    AddEditTruckDialog(QWidget* parent = nullptr);
    ~AddEditTruckDialog();

    void setEditMode(const QString& truckId);
    QString getTruckName() const;
    QString getLicensePlate() const;
    QString getTechnician() const;
    QString getStatus() const;
    QString getNotes() const;

private slots:
    void onSaveClicked();
    void onCancelClicked();

private:
    Ui::AddEditTruckDialogClass ui;

    void setupConnections();
    void loadTechnicianList();
};
