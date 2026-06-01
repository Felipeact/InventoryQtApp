#include "VerticalWidget.h"

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

    applyTheme(Theme::AppTheme::Dark);
    setUserInfo(userRole, userName);
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

void VerticalWidget::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::verticalBarStyle(theme)
    );
}