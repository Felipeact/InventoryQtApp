#pragma once

#include <QDialog>
#include <QString>

#include "ui_AddEditUserDialog.h"
#include "Theme.h"

class AddEditUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEditUserDialog(
        QWidget* parent = nullptr
    );

    ~AddEditUserDialog();

    void applyTheme(
        Theme::AppTheme theme
    );

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

    void setViewMode();

private:
    Ui::AddEditUserDialogClass ui;

    bool viewMode = false;

    void setupConnections();
    bool validateForm();

    void onSaveClicked();
    void onCancelClicked();
};