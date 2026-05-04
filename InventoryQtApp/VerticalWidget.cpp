// VerticalWidget.cpp - Implementation of the user information bar
#include "VerticalWidget.h"

// Constructor initializes the user info bar with role and name
VerticalWidget::VerticalWidget(const std::string& userRole, const std::string& userName, QWidget *parent)
	: QWidget(parent), role(userRole), username(userName)
{
	ui.setupUi(this);
}

// Destructor
VerticalWidget::~VerticalWidget()
{}

