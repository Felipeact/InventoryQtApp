#include "UsersPage.h"

#include <QComboBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <AddEditUserDialog.h>

UsersPage::UsersPage(QWidget* parent)
    : QWidget(parent)
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
    ui.usersTable->setRowCount(3);
    ui.usersTable->setColumnCount(6);

    ui.usersTable->setHorizontalHeaderLabels({
        "Name",
        "Email",
        "Role",
        "Status",
        "Created",
        "Actions"
        });

    ui.usersTable->setItem(0, 0, new QTableWidgetItem("Admin User"));
    ui.usersTable->setItem(0, 1, new QTableWidgetItem("admin@test.com"));
    ui.usersTable->setItem(0, 2, new QTableWidgetItem("ADMIN"));
    ui.usersTable->setItem(0, 3, new QTableWidgetItem("Active"));
    ui.usersTable->setItem(0, 4, new QTableWidgetItem("2026-05-01"));
    ui.usersTable->setItem(0, 5, new QTableWidgetItem("Edit / Delete"));

    ui.usersTable->setItem(1, 0, new QTableWidgetItem("Warehouse User"));
    ui.usersTable->setItem(1, 1, new QTableWidgetItem("warehouse@test.com"));
    ui.usersTable->setItem(1, 2, new QTableWidgetItem("WAREHOUSE"));
    ui.usersTable->setItem(1, 3, new QTableWidgetItem("Active"));
    ui.usersTable->setItem(1, 4, new QTableWidgetItem("2026-05-02"));
    ui.usersTable->setItem(1, 5, new QTableWidgetItem("Edit / Delete"));

    ui.usersTable->setItem(2, 0, new QTableWidgetItem("Technician User"));
    ui.usersTable->setItem(2, 1, new QTableWidgetItem("tech@test.com"));
    ui.usersTable->setItem(2, 2, new QTableWidgetItem("TECHNICIAN"));
    ui.usersTable->setItem(2, 3, new QTableWidgetItem("Active"));
    ui.usersTable->setItem(2, 4, new QTableWidgetItem("2026-05-03"));
    ui.usersTable->setItem(2, 5, new QTableWidgetItem("Edit / Delete"));

    ui.usersTable->horizontalHeader()->setStretchLastSection(true);
    ui.usersTable->verticalHeader()->setVisible(false);

    ui.paginationLabel->setText("Showing 1 to 3 of 3 users");
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
        int row = ui.usersTable->rowCount();
        ui.usersTable->insertRow(row);

        ui.usersTable->setItem(row, 0, new QTableWidgetItem(dialog.getName()));
        ui.usersTable->setItem(row, 1, new QTableWidgetItem(dialog.getEmail()));
        ui.usersTable->setItem(row, 2, new QTableWidgetItem(dialog.getRole()));
        ui.usersTable->setItem(row, 3, new QTableWidgetItem(dialog.getStatus()));
        ui.usersTable->setItem(row, 4, new QTableWidgetItem("Today"));
        ui.usersTable->setItem(row, 5, new QTableWidgetItem("Edit / Delete"));

        ui.paginationLabel->setText(
            QString("Showing 1 to %1 of %1 users").arg(ui.usersTable->rowCount())
        );
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