#include "VerticalWidget.h"

VerticalWidget::VerticalWidget(const std::string& userRole, const std::string& userName, QWidget *parent)
	: QWidget(parent), role(userRole), username(userName)
{
	ui.setupUi(this);
}

VerticalWidget::~VerticalWidget()
{}

