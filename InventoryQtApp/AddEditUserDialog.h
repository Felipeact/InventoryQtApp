#pragma once

#include <QDialog>
#include <QString>
#include <QPoint>

#include "ui_AddEditUserDialog.h"
#include "Theme.h"

class QMouseEvent;

class AddEditUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEditUserDialog(QWidget* parent = nullptr);
    ~AddEditUserDialog();

    void applyTheme(Theme::AppTheme theme);

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

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    Ui::AddEditUserDialogClass ui;

    bool viewMode = false;
    QPoint dragPosition;

    void setupConnections();
    bool validateForm();

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onCloseClicked();
};