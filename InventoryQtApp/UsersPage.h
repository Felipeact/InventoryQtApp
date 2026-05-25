#pragma once

#include <QWidget>
#include <QString>

#include "ui_UsersPage.h"

class UsersPage : public QWidget
{
public:
    explicit UsersPage(QWidget* parent = nullptr);
    ~UsersPage();

    void refreshUsers();

private:
    Ui::UsersPageClass ui;

    void setupConnections();
    void loadUsers();
    void filterUsers();

    void onAddUserClicked();
    void onSearchChanged(const QString& text);
    void onRoleFilterChanged(int index);
};