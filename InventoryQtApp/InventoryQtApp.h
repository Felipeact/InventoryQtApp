#pragma once
#include <DashboardWindow.h>

#include <QtWidgets/QMainWindow>
#include "ui_InventoryQtApp.h"

#include "ApiClient.h"
#include "AuthService.h"


class InventoryQtApp : public QMainWindow
{
    Q_OBJECT

public:
    InventoryQtApp(QWidget *parent = nullptr);
    ~InventoryQtApp();

private slots:
    void onLoginButtonClicked();

private:
    Ui::InventoryQtAppClass ui;
    
    ApiClient apiClient;
    AuthService authService;

	DashboardWindow* dashboardWindow = nullptr;


 
};

