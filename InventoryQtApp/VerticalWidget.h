#pragma once

#include <QWidget>
#include <string>
#include "ui_VerticalWidget.h"

class VerticalWidget : public QWidget
{
	Q_OBJECT

public:
	VerticalWidget(const std::string& userRole, const std::string& userName, QWidget *parent = nullptr);
	~VerticalWidget();

private:
	Ui::VerticalWidgetClass ui;

	const std::string role;
	const std::string username;

};

