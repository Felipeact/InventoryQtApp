#pragma once

#include <QWidget>
#include <QString>
#include <QPushButton>


#include "UserService.h"

#include "ui_UsersPage.h"

class UsersPage : public QWidget
{
public:
    explicit UsersPage(UserService* userService, QWidget* parent = nullptr);
    ~UsersPage();

    void refreshUsers();

private:
    Ui::UsersPageClass ui;

	UserService* userService = nullptr;

    std::vector<UserDto> currentUsers;

    void setupConnections();
    void loadUsers();
    void filterUsers();

    void onAddUserClicked();
    void onSearchChanged(const QString& text);
    void onRoleFilterChanged(int index);

    void addActionButtons(int row);
    void onEditUserClicked(int row);
    void onDeleteUserClicked(int row);
};