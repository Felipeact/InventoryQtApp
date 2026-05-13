// InventoryQtApp.cpp - Implementation of the login window
#include "InventoryQtApp.h"
#include <QMessageBox>

// Constructor initializes the login window and sets up API client
InventoryQtApp::InventoryQtApp(QWidget *parent)
    : 
	QMainWindow(parent),
	apiClient("https://fluffy-succotash-4wg97gqvwxq27p7v-3000.app.github.dev"),
	authService(apiClient)
{
    ui.setupUi(this);

	this->setWindowTitle("Inventory Management System - Login");
	this->resize(420, 320);
	this->setMaximumSize(420, 320);

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
		ui.statusLabel->setText("Login failed.");
		QMessageBox::warning(
			this,
			"Login Failed",
			QString::fromStdString(loginResult.errorMessage)
		);
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
		this->show();
		});

	dashboardWindow->showMaximized();
	this->hide();
}

