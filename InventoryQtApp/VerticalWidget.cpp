// VerticalWidget.cpp - Implementation of the user information bar
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
}