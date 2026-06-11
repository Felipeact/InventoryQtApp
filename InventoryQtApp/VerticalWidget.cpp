// VerticalWidget.cpp - Implementation of the user information bar
#include "VerticalWidget.h"
#include "Theme.h"

#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

VerticalWidget::VerticalWidget(
    const std::string& userRole,
    const std::string& userName,
    QWidget* parent
)
    : QWidget(parent),
    role(userRole),
    username(userName)
{
    ui.setupUi(this);

    setUserInfo(userRole, userName);

    searchDebounceTimer = new QTimer(this);
    searchDebounceTimer->setSingleShot(true);
    searchDebounceTimer->setInterval(400);

    connect(
        searchDebounceTimer,
        &QTimer::timeout,
        this,
        [this]() {
            emit globalSearchTextChanged(pendingSearchText);
        }
    );

    connect(
        ui.searchInput,
        &QLineEdit::textChanged,
        this,
        [this](const QString& text) {
            pendingSearchText = text;
            searchDebounceTimer->start();
        }
    );

    connect(
        ui.notificationButton,
        &QPushButton::clicked,
        this,
        [this]() {
            emit notificationRequested();
        }
    );
}

VerticalWidget::~VerticalWidget()
{
}

void VerticalWidget::setUserInfo(
    const std::string& role,
    const std::string& userName
)
{
    this->role = role;
    this->username = userName;

    ui.welcomeLabel->setText(
        "Hello " + QString::fromStdString(userName)
    );

    if (!userName.empty()) {
        ui.avatarLabel->setText(
            QString::fromStdString(userName.substr(0, 1)).toUpper()
        );
    }
}

void VerticalWidget::clearSearch()
{
    ui.searchInput->clear();
}

void VerticalWidget::focusSearch()
{
    ui.searchInput->setFocus();
    ui.searchInput->selectAll();
}

void VerticalWidget::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::verticalBarStyle(theme)
    );
}