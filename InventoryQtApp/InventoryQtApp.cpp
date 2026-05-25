// InventoryQtApp.cpp - Implementation of the login window
#include "InventoryQtApp.h"
#include <Theme.h>
#include <QMessageBox>

#include "Config.h"

// Constructor initializes the login window and sets up API client
InventoryQtApp::InventoryQtApp(QWidget *parent)
    : 
	QMainWindow(parent),
	apiClient(Config::API_BASE_URL),
	authService(apiClient)
{
    ui.setupUi(this);

	this->setWindowTitle("Inventory Management System - Login");
	Theme::applyLogin(this);
	this->resize(420, 320);
	this->setMaximumSize(420, 320);

	ui.emailInput->setPlaceholderText("Enter your email");
	ui.passwordInput->setPlaceholderText("Enter your password");
	this->setTabOrder(ui.emailInput, ui.passwordInput);
	this->setTabOrder(ui.passwordInput, ui.rememberCheck);
	this->setTabOrder(ui.rememberCheck, ui.forgotPasswordButton);
	this->setTabOrder(ui.forgotPasswordButton, ui.loginButton);

	ui.passwordInput->setEchoMode(QLineEdit::EchoMode::Password);
	ui.statusLabel->setText("");

	

	connect(
		ui.loginButton, &QPushButton::clicked,
		this, &InventoryQtApp::onLoginButtonClicked
	);
}

// Destructor
InventoryQtApp::~InventoryQtApp()
{}

// Processes login by validating credentials and opening the dashboard
void InventoryQtApp::onLoginButtonClicked()
{
	QString email = ui.emailInput->text();
	QString password = ui.passwordInput->text();

	if (email.isEmpty() || password.isEmpty()) {
		ui.statusLabel->setText("Email and password are required.");
		return;
	}

	if (password.length() < 6) {
		ui.statusLabel->setText("Password must be at least 6 characters.");
		return;
	}

	ui.statusLabel->setText("Logging in...");

	LoginResult loginResult = authService.login(
		email.toStdString(),
		password.toStdString()
	);

	if (!loginResult.success) {
		ui.statusLabel->setText("Login failed: " + QString::fromStdString(loginResult.errorMessage));
		
		return;
	}

	apiClient.setAccessToken(loginResult.accessToken);
	apiClient.setRefreshToken(loginResult.refreshToken);

	std::string role;
	std::vector<std::string> permissions;

	if (!apiClient.validateToken(role, permissions)) {
		ui.statusLabel->setText("Token validation failed.");
		QMessageBox::warning(this, "Error", "Token validation failed.");
		return;
	}

		
	dashboardWindow = new DashboardWindow(role,permissions,productService,assetService, reportService);

	connect(dashboardWindow, &DashboardWindow::logoutRequested, this, [this]() {
		authService.logout();
		ui.statusLabel->setText("");
		this->show();
		});

	dashboardWindow->showMaximized();
	this->hide();
}

