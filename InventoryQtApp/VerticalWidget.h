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

private:
	Ui::VerticalWidgetClass ui;  // UI components

	const std::string role;      // User's role
	const std::string username;  // User's name

};

