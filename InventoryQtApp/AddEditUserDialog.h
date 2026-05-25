#pragma once

#include <QDialog>
#include <QString>

#include "ui_AddEditUserDialog.h"

class AddEditUserDialog : public QDialog
{
public:
    explicit AddEditUserDialog(QWidget* parent = nullptr);
    ~AddEditUserDialog();

    QString getName() const;
    QString getEmail() const;
    QString getPassword() const;
    QString getRole() const;
    QString getStatus() const;

    void setUser(
        const QString& name,
        const QString& email,
        const QString& role,
        const QString& status
    );

private:
    Ui::AddEditUserDialogClass ui;

    void setupConnections();
    bool validateForm();

    void onSaveClicked();
    void onCancelClicked();
};