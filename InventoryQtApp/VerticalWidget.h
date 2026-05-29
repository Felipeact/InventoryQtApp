// VerticalWidget.h - Top bar displaying user information
#pragma once

#include <QWidget>
#include <string>
#include "ui_VerticalWidget.h"

// Widget that displays user role and name in the top bar
class VerticalWidget : public QWidget
{
	Q_OBJECT

public:
	// Constructor that initializes the user info bar with role and name
	VerticalWidget(const std::string& userRole, const std::string& userName, QWidget *parent = nullptr);
	// Destructor
	~VerticalWidget();

	void setUserInfo(const std::string& role, const std::string& userName);

private:
	Ui::VerticalWidget ui;  // UI components

	std::string role;
	std::string username;

};

