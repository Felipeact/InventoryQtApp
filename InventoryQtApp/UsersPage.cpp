#include "UsersPage.h"
#include "Theme.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QComboBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>

#include "AddEditUserDialog.h"

UsersPage::UsersPage(
    UserService* userService,
    QWidget* parent
)
    : QWidget(parent),
    userService(userService)
{
    ui.setupUi(this);

    applyTheme(Theme::AppTheme::Dark);

    setupTable();
    setupConnections();
    loadUsers();
}

UsersPage::~UsersPage()
{
}

void UsersPage::setupConnections()
{
    connect(
        ui.addUserButton,
        &QPushButton::clicked,
        this,
        &UsersPage::onAddUserClicked
    );

    connect(
        ui.searchInput,
        &QLineEdit::textChanged,
        this,
        &UsersPage::onSearchChanged
    );

    connect(
        ui.roleFilter,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &UsersPage::onRoleFilterChanged
    );

    connect(
        ui.pageButton,
        &QPushButton::clicked,
        this,
        &UsersPage::onPreviousPageClicked
    );

    connect(
        ui.pageButton3,
        &QPushButton::clicked,
        this,
        &UsersPage::onNextPageClicked
    );

    connect(
        ui.pageButton2,
        &QPushButton::clicked,
        this,
        &UsersPage::onPage2Clicked
    );
}

void UsersPage::setupTable()
{
    ui.usersTable->setColumnCount(6);

    ui.usersTable->setHorizontalHeaderLabels(
        QStringList()
        << "Name"
        << "Email"
        << "Role"
        << "Status"
        << "Created"
        << "Actions"
    );

    ui.usersTable->verticalHeader()->setVisible(false);
    ui.usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.usersTable->setShowGrid(false);
    ui.usersTable->setFrameShape(QFrame::NoFrame);
    ui.usersTable->setFocusPolicy(Qt::NoFocus);
    ui.usersTable->setAlternatingRowColors(false);
    ui.usersTable->viewport()->setAutoFillBackground(false);

    ui.usersTable->horizontalHeader()->setHighlightSections(false);
    ui.usersTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    ui.usersTable->horizontalHeader()->setFixedHeight(48);
    ui.usersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.usersTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);

    ui.usersTable->setColumnWidth(5, 130);
    ui.usersTable->verticalHeader()->setDefaultSectionSize(52);

    ui.usersTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.usersTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void UsersPage::loadUsers()
{
    if (!userService) {
        return;
    }

    currentUsers = userService->getUsers();

    filterUsers();
}

void UsersPage::refreshUsers()
{
    loadUsers();
}

void UsersPage::setLoggedInUserName(
    const std::string& userName
)
{
    loggedInUserName = userName;
}

void UsersPage::filterUsers()
{
    QString searchText =
        ui.searchInput->text().trimmed();

    QString selectedRole =
        ui.roleFilter->currentText();

    filteredUsers.clear();

    for (const UserDto& user : currentUsers) {
        QString name =
            QString::fromStdString(user.name);

        QString email =
            QString::fromStdString(user.email);

        QString role =
            QString::fromStdString(user.role);

        QString status =
            QString::fromStdString(user.status);

        bool matchesSearch =
            searchText.isEmpty() ||
            name.contains(searchText, Qt::CaseInsensitive) ||
            email.contains(searchText, Qt::CaseInsensitive) ||
            role.contains(searchText, Qt::CaseInsensitive) ||
            status.contains(searchText, Qt::CaseInsensitive);

        bool matchesRole =
            selectedRole == "All Roles" ||
            role.compare(selectedRole, Qt::CaseInsensitive) == 0;

        if (matchesSearch && matchesRole) {
            filteredUsers.push_back(user);
        }
    }

    currentPage = 1;

    populateTable();
    updatePagination();
}

void UsersPage::populateTable()
{
    ui.usersTable->clearContents();

    int totalItems =
        static_cast<int>(filteredUsers.size());

    int startIndex =
        (currentPage - 1) * pageSize;

    int endIndex =
        (std::min)(startIndex + pageSize, totalItems);

    int rowCount =
        endIndex - startIndex;

    ui.usersTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const UserDto& user =
            filteredUsers[startIndex + row];

        QTableWidgetItem* nameItem =
            new QTableWidgetItem(QString::fromStdString(user.name));

        nameItem->setData(
            Qt::UserRole,
            QString::fromStdString(user.id)
        );

        ui.usersTable->setItem(row, 0, nameItem);

        ui.usersTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(user.email))
        );

        ui.usersTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::fromStdString(user.role))
        );

        ui.usersTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::fromStdString(user.status))
        );

        ui.usersTable->setItem(
            row,
            4,
            new QTableWidgetItem(QString::fromStdString(user.createdAt))
        );

        addActionButtons(row, user.id);
    }
}

void UsersPage::updatePagination()
{
    int totalItems =
        static_cast<int>(filteredUsers.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage > totalPages) {
        currentPage = totalPages;
    }

    int startItem =
        totalItems == 0
        ? 0
        : ((currentPage - 1) * pageSize) + 1;

    int endItem =
        (std::min)(currentPage * pageSize, totalItems);

    ui.paginationLabel->setText(
        QString("Showing %1 to %2 of %3 users")
        .arg(startItem)
        .arg(endItem)
        .arg(totalItems)
    );

    ui.activePageButton->setText(QString::number(currentPage));
    ui.pageButton2->setText(QString::number(currentPage + 1));

    ui.pageButton->setEnabled(currentPage > 1);
    ui.pageButton3->setEnabled(currentPage < totalPages);
    ui.pageButton2->setVisible(currentPage < totalPages);
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
            QMessageBox::information(
                this,
                "Success",
                "User created successfully."
            );

            loadUsers();
        }
        else {
            QMessageBox::warning(
                this,
                "Error",
                "Failed to create user. Check the backend console."
            );
        }
    }
}

void UsersPage::onSearchChanged(
    const QString& text
)
{
    Q_UNUSED(text);

    filterUsers();
}

void UsersPage::onRoleFilterChanged(
    int index
)
{
    Q_UNUSED(index);

    filterUsers();
}

void UsersPage::onPreviousPageClicked()
{
    if (currentPage > 1) {
        currentPage--;

        populateTable();
        updatePagination();
    }
}

void UsersPage::onNextPageClicked()
{
    int totalItems =
        static_cast<int>(filteredUsers.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage < totalPages) {
        currentPage++;

        populateTable();
        updatePagination();
    }
}

void UsersPage::onPage2Clicked()
{
    int totalItems =
        static_cast<int>(filteredUsers.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage + 1 <= totalPages) {
        currentPage++;

        populateTable();
        updatePagination();
    }
}

void UsersPage::addActionButtons(
    int row,
    const std::string& userId
)
{
    QWidget* actionWidget =
        new QWidget(this);

    actionWidget->setObjectName("actionContainer");

    QHBoxLayout* layout =
        new QHBoxLayout(actionWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->setAlignment(Qt::AlignCenter);

    QPushButton* viewButton =
        new QPushButton("👁", actionWidget);

    viewButton->setObjectName("viewButton");

    QPushButton* editButton =
        new QPushButton("✎", actionWidget);

    editButton->setObjectName("editButton");

    QPushButton* deleteButton =
        new QPushButton("🗑", actionWidget);

    deleteButton->setObjectName("deleteButton");

    layout->addWidget(viewButton);
    layout->addWidget(editButton);
    layout->addWidget(deleteButton);

    ui.usersTable->setCellWidget(row, 5, actionWidget);

    connect(viewButton, &QPushButton::clicked, this, [this, userId]() {
        onViewUserClicked(userId);
        });

    connect(editButton, &QPushButton::clicked, this, [this, userId]() {
        onEditUserClicked(userId);
        });

    connect(deleteButton, &QPushButton::clicked, this, [this, userId]() {
        onDeleteUserClicked(userId);
        });
}

void UsersPage::onViewUserClicked(
    const std::string& userId
)
{
    auto it =
        std::find_if(
            currentUsers.begin(),
            currentUsers.end(),
            [&userId](const UserDto& user) {
                return user.id == userId;
            }
        );

    if (it == currentUsers.end()) {
        return;
    }

    const UserDto& user =
        *it;

    AddEditUserDialog dialog(this);

    dialog.setUser(
        QString::fromStdString(user.name),
        QString::fromStdString(user.email),
        QString::fromStdString(user.role),
        QString::fromStdString(user.status)
    );

    dialog.setViewMode();

    dialog.exec();
}

void UsersPage::onEditUserClicked(
    const std::string& userId
)
{
    auto it =
        std::find_if(
            currentUsers.begin(),
            currentUsers.end(),
            [&userId](const UserDto& user) {
                return user.id == userId;
            }
        );

    if (it == currentUsers.end()) {
        return;
    }

    const UserDto& user =
        *it;

    AddEditUserDialog dialog(this);

    dialog.setUser(
        QString::fromStdString(user.name),
        QString::fromStdString(user.email),
        QString::fromStdString(user.role),
        QString::fromStdString(user.status)
    );

    if (dialog.exec() == QDialog::Accepted) {
        UpdateUserRequest request;

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
            userService->updateUser(user.id, request);

        if (success) {
            QMessageBox::information(
                this,
                "Success",
                "User updated successfully."
            );

            if (user.name == loggedInUserName) {
                emit loggedInUserUpdated(request.name);
            }

            loadUsers();
        }
        else {
            QMessageBox::warning(
                this,
                "Error",
                "Failed to update user."
            );
        }
    }
}

void UsersPage::onDeleteUserClicked(
    const std::string& userId
)
{
    auto it =
        std::find_if(
            currentUsers.begin(),
            currentUsers.end(),
            [&userId](const UserDto& user) {
                return user.id == userId;
            }
        );

    if (it == currentUsers.end()) {
        return;
    }

    const UserDto& user =
        *it;

    QMessageBox::StandardButton confirm =
        QMessageBox::question(
            this,
            "Delete User",
            "Are you sure you want to delete this user?",
            QMessageBox::Yes | QMessageBox::No
        );

    if (confirm != QMessageBox::Yes) {
        return;
    }

    bool success =
        userService->deleteUser(user.id);

    if (success) {
        QMessageBox::information(
            this,
            "Success",
            "User deleted successfully."
        );

        loadUsers();
    }
    else {
        QMessageBox::warning(
            this,
            "Error",
            "Failed to delete user."
        );
    }
}

void UsersPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::usersPageStyle(theme)
    );
}

void UsersPage::setSearchText(const QString& text)
{
    ui.searchInput->setText(text);
}