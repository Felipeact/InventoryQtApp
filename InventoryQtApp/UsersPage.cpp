#include "UsersPage.h"

#include <QComboBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <AddEditUserDialog.h>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>



UsersPage::UsersPage(UserService* userService, QWidget* parent)
    : QWidget(parent), userService(userService)
{
    ui.setupUi(this);
    setupConnections();
    loadUsers();
}

UsersPage::~UsersPage()
{
}

void UsersPage::setupConnections()
{
    connect(ui.addUserButton, &QPushButton::clicked,
        this, &UsersPage::onAddUserClicked);

    connect(ui.searchInput, &QLineEdit::textChanged,
        this, &UsersPage::onSearchChanged);

    connect(ui.roleFilter,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &UsersPage::onRoleFilterChanged);
}

void UsersPage::loadUsers()
{
    ui.usersTable->clearContents();

    currentUsers = userService->getUsers();

    ui.usersTable->setRowCount(
        static_cast<int>(currentUsers.size())
    );

    for (int row = 0; row < currentUsers.size(); ++row) {

        const UserDto& user = currentUsers[row];

        ui.usersTable->setItem(
            row, 0,
            new QTableWidgetItem(
                QString::fromStdString(user.name)
            )
        );

        ui.usersTable->setItem(
            row, 1,
            new QTableWidgetItem(
                QString::fromStdString(user.email)
            )
        );

        ui.usersTable->setItem(
            row, 2,
            new QTableWidgetItem(
                QString::fromStdString(user.role)
            )
        );

        ui.usersTable->setItem(
            row, 3,
            new QTableWidgetItem(
                QString::fromStdString(user.status)
            )
        );

        ui.usersTable->setItem(
            row, 4,
            new QTableWidgetItem(
                QString::fromStdString(user.createdAt)
            )
        );

        addActionButtons(row);
    }

    ui.paginationLabel->setText(
        QString("Showing %1 users")
        .arg(currentUsers.size())
    );
}

void UsersPage::refreshUsers()
{
    loadUsers();
}

void UsersPage::filterUsers()
{
    QString searchText = ui.searchInput->text().trimmed();
    QString selectedRole = ui.roleFilter->currentText();

    for (int row = 0; row < ui.usersTable->rowCount(); ++row) {
        bool searchMatch = searchText.isEmpty();
        bool roleMatch = selectedRole == "All Roles";

        for (int col = 0; col < ui.usersTable->columnCount(); ++col) {
            QTableWidgetItem* item = ui.usersTable->item(row, col);

            if (item && item->text().contains(searchText, Qt::CaseInsensitive)) {
                searchMatch = true;
            }
        }

        QTableWidgetItem* roleItem = ui.usersTable->item(row, 2);

        if (roleItem && roleItem->text() == selectedRole) {
            roleMatch = true;
        }

        ui.usersTable->setRowHidden(row, !(searchMatch && roleMatch));
    }
}

void UsersPage::onAddUserClicked()
{
    AddEditUserDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {

        CreateUserRequest request;

        request.name =
            dialog.getName().toStdString();

        request.email =
            dialog.getEmail().toStdString();

        request.password =
            dialog.getPassword().toStdString();

        request.role =
            dialog.getRole().toStdString();

        request.status =
            dialog.getStatus().toStdString();

        bool success =
            userService->createUser(request);

        if (success) {
            QMessageBox::information(this, "Success", "User created successfully.");
            loadUsers();
        }
        else {
            QMessageBox::warning(this, "Error", "Failed to create user. Check the backend console.");
        }
    }
}

void UsersPage::onSearchChanged(const QString& text)
{
    Q_UNUSED(text);
    filterUsers();
}

void UsersPage::onRoleFilterChanged(int index)
{
    Q_UNUSED(index);
    filterUsers();
}

void UsersPage::addActionButtons(int row)
{
    QWidget* actionWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(actionWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    QPushButton* editButton = new QPushButton("Edit", actionWidget);
    QPushButton* deleteButton = new QPushButton("Delete", actionWidget);

    editButton->setMinimumHeight(28);
    deleteButton->setMinimumHeight(28);

    layout->addWidget(editButton);
    layout->addWidget(deleteButton);
    layout->addStretch();

    ui.usersTable->setCellWidget(row, 5, actionWidget);

    connect(editButton, &QPushButton::clicked, this, [this, row]() {
        onEditUserClicked(row);
        });

    connect(deleteButton, &QPushButton::clicked, this, [this, row]() {
        onDeleteUserClicked(row);
        });
}

void UsersPage::onEditUserClicked(int row)
{
    if (row < 0 || row >= static_cast<int>(currentUsers.size())) {
        return;
    }

    const UserDto& user = currentUsers[row];

    AddEditUserDialog dialog(this);

    dialog.setUser(
        QString::fromStdString(user.name),
        QString::fromStdString(user.email),
        QString::fromStdString(user.role),
        QString::fromStdString(user.status)
    );

    if (dialog.exec() == QDialog::Accepted) {

        UpdateUserRequest request;

        request.name = dialog.getName().toStdString();
        request.email = dialog.getEmail().toStdString();
        request.password = dialog.getPassword().toStdString();
        request.role = dialog.getRole().toStdString();
        request.status = dialog.getStatus().toStdString();

        bool success = userService->updateUser(user.id, request);

        if (success) {
            QMessageBox::information(this, "Success", "User updated successfully.");
            loadUsers();
        }
        else {
            QMessageBox::warning(this, "Error", "Failed to update user.");
        }
    }
}

void UsersPage::onDeleteUserClicked(int row)
{
    if (row < 0 || row >= static_cast<int>(currentUsers.size())) {
        return;
    }

    const UserDto& user = currentUsers[row];

    QMessageBox::StandardButton confirm =
        QMessageBox::question(
            this,
            "Delete User",
            "Are you sure you want to delete this user?"
        );

    if (confirm != QMessageBox::Yes) {
        return;
    }

    bool success = userService->deleteUser(user.id);

    if (success) {
        QMessageBox::information(this, "Success", "User deleted successfully.");
        loadUsers();
    }
    else {
        QMessageBox::warning(this, "Error", "Failed to delete user.");
    }
}
