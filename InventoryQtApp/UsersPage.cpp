#include "UsersPage.h"
#include "IconUtil.h"
#include "Theme.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QComboBox>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>
#include <QThread>
#include <QPointer>
#include <QMetaObject>

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
    ui.usersTable->setColumnCount(7);

    ui.usersTable->setHorizontalHeaderLabels(
        QStringList()
        << "Name"
        << "Email"
        << "Role"
        << "Status"
        << "Created"
        << "Actions"
        << "Security"
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

    ui.usersTable->horizontalHeader()->setSectionResizeMode(
        5,
        QHeaderView::Fixed
    );

    ui.usersTable->horizontalHeader()->setSectionResizeMode(
        6,
        QHeaderView::Fixed
    );

    ui.usersTable->setColumnWidth(5, 130);
    ui.usersTable->setColumnWidth(6, 140);
    ui.usersTable->verticalHeader()->setDefaultSectionSize(52);

    ui.usersTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.usersTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void UsersPage::loadUsers()
{
    if (!userService) {
        return;
    }

    QPointer<UsersPage> self(this);
    UserService* service = userService;

    QThread* worker =
        QThread::create([self, service]() {
        std::vector<UserDto> users =
            service->getUsers();

        if (!self) {
            return;
        }

        QMetaObject::invokeMethod(
            self,
            [self, users]() {
                if (!self) {
                    return;
                }

                self->currentUsers = users;
                self->filterUsers();
            },
            Qt::QueuedConnection
        );
            });

    connect(
        worker,
        &QThread::finished,
        worker,
        &QObject::deleteLater
    );

    worker->start();
}

void UsersPage::refreshUsers()
{
    loadUsers();
}

void UsersPage::setLoggedInUserName(
    const std::string& userName
)
{
    loggedInUserName =
        userName;
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
            role.compare(
                selectedRole,
                Qt::CaseInsensitive
            ) == 0;

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
    ui.usersTable->clearSpans();

    int totalItems =
        static_cast<int>(filteredUsers.size());

    int startIndex =
        (currentPage - 1) * pageSize;

    int endIndex =
        (std::min)(startIndex + pageSize, totalItems);

    int rowCount =
        endIndex - startIndex;

    if (rowCount == 0) {
        ui.usersTable->setRowCount(1);

        ui.usersTable->setItem(
            0,
            0,
            new QTableWidgetItem("No users found")
        );

        ui.usersTable->setSpan(
            0,
            0,
            1,
            7
        );

        return;
    }

    ui.usersTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const UserDto& user =
            filteredUsers[startIndex + row];

        QTableWidgetItem* nameItem =
            new QTableWidgetItem(
                QString::fromStdString(user.name)
            );

        nameItem->setData(
            Qt::UserRole,
            QString::fromStdString(user.id)
        );

        ui.usersTable->setItem(
            row,
            0,
            nameItem
        );

        ui.usersTable->setItem(
            row,
            1,
            new QTableWidgetItem(
                QString::fromStdString(user.email)
            )
        );

        ui.usersTable->setItem(
            row,
            2,
            new QTableWidgetItem(
                QString::fromStdString(user.role)
            )
        );

        ui.usersTable->setItem(
            row,
            3,
            new QTableWidgetItem(
                QString::fromStdString(user.status)
            )
        );

        ui.usersTable->setItem(
            row,
            4,
            new QTableWidgetItem(
                QString::fromStdString(user.createdAt)
            )
        );

        addActionButtons(
            row,
            user.id
        );
    }

    ui.usersTable->setUpdatesEnabled(true);
}

void UsersPage::updatePagination()
{
    int totalItems =
        static_cast<int>(filteredUsers.size());

    int totalPages =
        (std::max)(
            1,
            (totalItems + pageSize - 1) / pageSize
            );

    if (currentPage > totalPages) {
        currentPage = totalPages;
    }

    int startItem =
        totalItems == 0
        ? 0
        : ((currentPage - 1) * pageSize) + 1;

    int endItem =
        (std::min)(
            currentPage * pageSize,
            totalItems
            );

    ui.paginationLabel->setText(
        QString("Showing %1 to %2 of %3 users")
        .arg(startItem)
        .arg(endItem)
        .arg(totalItems)
    );

    ui.activePageButton->setText(
        QString::number(currentPage)
    );

    ui.pageButton2->setText(
        QString::number(currentPage + 1)
    );

    ui.pageButton->setEnabled(
        currentPage > 1
    );

    ui.pageButton3->setEnabled(
        currentPage < totalPages
    );

    ui.pageButton2->setVisible(
        currentPage < totalPages
    );
}

void UsersPage::onAddUserClicked()
{
    if (!userService) {
        QMessageBox::warning(
            this,
            "Error",
            "User service is unavailable."
        );

        return;
    }

    AddEditUserDialog dialog(this);
    dialog.setInviteMode();

    if (dialog.exec() == QDialog::Accepted) {
        CreateUserRequest request;

        request.name =
            dialog.getName().toStdString();

        request.email =
            dialog.getEmail().toStdString();

        request.password = "";

        request.role =
            dialog.getRole().toStdString();

        request.status =
            dialog.getStatus().toStdString();

        std::string inviteResult =
            userService->inviteUser(request);

        if (!inviteResult.empty()) {
            if (inviteResult == "__EMAIL_SENT__") {
                QMessageBox::information(
                    this,
                    "User Invited",
                    QString(
                        "User invited successfully.\n\n"
                        "Name: %1\n"
                        "Email: %2\n\n"
                        "An email was sent with login instructions."
                    )
                    .arg(dialog.getName())
                    .arg(dialog.getEmail())
                );
            }
            else {
                QMessageBox::information(
                    this,
                    "User Invited",
                    QString(
                        "User invited successfully.\n\n"
                        "Name: %1\n"
                        "Email: %2\n\n"
                        "Temporary password:\n%3\n\n"
                        "Email sending failed, so give this password to the user."
                    )
                    .arg(dialog.getName())
                    .arg(dialog.getEmail())
                    .arg(QString::fromStdString(inviteResult))
                );
            }

            loadUsers();
        }
        else {
            QMessageBox::warning(
                this,
                "Error",
                "Failed to invite user. Check the backend console."
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
        (std::max)(
            1,
            (totalItems + pageSize - 1) / pageSize
            );

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
        (std::max)(
            1,
            (totalItems + pageSize - 1) / pageSize
            );

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

    QHBoxLayout* actionLayout =
        new QHBoxLayout(actionWidget);

    actionLayout->setContentsMargins(0, 0, 0, 0);
    actionLayout->setSpacing(6);
    actionLayout->setAlignment(Qt::AlignCenter);

    QPushButton* viewButton =
        new QPushButton("👁", actionWidget);

    viewButton->setObjectName("viewButton");
    IconUtil::iconifyActionButton(viewButton);

    QPushButton* editButton =
        new QPushButton("✎", actionWidget);

    editButton->setObjectName("editButton");
    IconUtil::iconifyActionButton(editButton);

    QPushButton* deleteButton =
        new QPushButton("🗑", actionWidget);

    deleteButton->setObjectName("deleteButton");
    IconUtil::iconifyActionButton(deleteButton);

    actionLayout->addWidget(viewButton);
    actionLayout->addWidget(editButton);
    actionLayout->addWidget(deleteButton);

    ui.usersTable->setCellWidget(
        row,
        5,
        actionWidget
    );

    QWidget* securityWidget =
        new QWidget(this);

    securityWidget->setObjectName("securityContainer");

    QHBoxLayout* securityLayout =
        new QHBoxLayout(securityWidget);

    securityLayout->setContentsMargins(0, 0, 0, 0);
    securityLayout->setSpacing(6);
    securityLayout->setAlignment(Qt::AlignCenter);

    QPushButton* resetButton =
        new QPushButton("🔑", securityWidget);

    resetButton->setObjectName("resetButton");
    IconUtil::iconifyActionButton(resetButton);
    resetButton->setToolTip("Reset Password");

    securityLayout->addWidget(resetButton);

    ui.usersTable->setCellWidget(
        row,
        6,
        securityWidget
    );

    connect(
        viewButton,
        &QPushButton::clicked,
        this,
        [this, userId]() {
            onViewUserClicked(userId);
        }
    );

    connect(
        editButton,
        &QPushButton::clicked,
        this,
        [this, userId]() {
            onEditUserClicked(userId);
        }
    );

    connect(
        deleteButton,
        &QPushButton::clicked,
        this,
        [this, userId]() {
            onDeleteUserClicked(userId);
        }
    );

    connect(
        resetButton,
        &QPushButton::clicked,
        this,
        [this, userId]() {
            onResetPasswordClicked(userId);
        }
    );
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
            userService->updateUser(
                user.id,
                request
            );

        if (success) {
            QMessageBox::information(
                this,
                "Success",
                "User updated successfully."
            );

            if (user.name == loggedInUserName) {
                emit loggedInUserUpdated(
                    request.name
                );
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

void UsersPage::onResetPasswordClicked(
    const std::string& userId
)
{
    if (!userService) {
        QMessageBox::warning(
            this,
            "Error",
            "User service is unavailable."
        );

        return;
    }

    auto it =
        std::find_if(
            currentUsers.begin(),
            currentUsers.end(),
            [&userId](const UserDto& user) {
                return user.id == userId;
            }
        );

    QString userName =
        it != currentUsers.end()
        ? QString::fromStdString(it->name)
        : "this user";

    QString userEmail =
        it != currentUsers.end()
        ? QString::fromStdString(it->email)
        : "";

    QString message =
        "Generate a temporary password for " + userName;

    if (!userEmail.isEmpty()) {
        message += "\n\nEmail: " + userEmail;
    }

    message += "\n\nThe old password will stop working.";

    QMessageBox::StandardButton confirm =
        QMessageBox::question(
            this,
            "Reset Password",
            message,
            QMessageBox::Yes | QMessageBox::No
        );

    if (confirm != QMessageBox::Yes) {
        return;
    }

    std::string temporaryPassword =
        userService->resetPassword(userId);

    if (temporaryPassword.empty()) {
        QMessageBox::warning(
            this,
            "Reset Failed",
            "Password reset failed. Check the backend console."
        );

        return;
    }

    QMessageBox::information(
        this,
        "Temporary Password",
        QString(
            "Temporary password generated successfully.\n\n"
            "User: %1\n"
            "Email: %2\n\n"
            "Temporary password:\n%3\n\n"
            "Give this password to the user and ask them to change it after login."
        )
        .arg(userName)
        .arg(userEmail.isEmpty() ? "-" : userEmail)
        .arg(QString::fromStdString(temporaryPassword))
    );
}

void UsersPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::usersPageStyle(theme)
    );
}

void UsersPage::setSearchText(
    const QString& text
)
{
    ui.searchInput->setText(text);
}