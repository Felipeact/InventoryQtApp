#pragma once

#include <QWidget>
#include "ui_DashboardPage.h"

class DashboardPage : public QWidget
{
	Q_OBJECT

public:
	DashboardPage(QWidget *parent = nullptr);
	~DashboardPage();

private:
	Ui::DashboardPageClass ui;

	// Dasboard page 
	void setupItemListTable();
};

