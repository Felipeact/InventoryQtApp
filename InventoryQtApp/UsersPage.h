#pragma once

#include <QWidget>
#include <QString>
#include <QPushButton>
#include <vector>

#include "UserService.h"
#include "ui_UsersPage.h"
#include "Theme.h"

class UsersPage : public QWidget
{

    Q_OBJECT

public:

    void applyTheme(Theme::AppTheme theme);
    explicit UsersPage(UserService* userService, QWidget* parent = nullptr);
    ~UsersPage();

    void refreshUsers();

    void setLoggedInUserName(
        const std::string& userName
    );

signals:
    void loggedInUserUpdated(
        const std::string& newUserName
    );

private:
    Ui::UsersPageClass ui;

    UserService* userService = nullptr;

    std::string loggedInUserName;
    std::vector<UserDto> currentUsers;
    std::vector<UserDto> filteredUsers;

    int currentPage = 1;
    int pageSize = 10;

    void setupConnections();
    void loadUsers();
    void filterUsers();
    void populateTable();
    void updatePagination();

    void onAddUserClicked();
    void onSearchChanged(const QString& text);
    void onRoleFilterChanged(int index);

    void onPreviousPageClicked();
    void onNextPageClicked();
    void onPage2Clicked();

    void addActionButtons(int row, const std::string& userId);
    void onEditUserClicked(const std::string& userId);
    void onDeleteUserClicked(const std::string& userId);
};