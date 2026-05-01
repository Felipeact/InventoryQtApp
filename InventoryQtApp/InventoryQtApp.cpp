#include "InventoryQtApp.h"
#include <QMessageBox>


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

InventoryQtApp::~InventoryQtApp()
{}

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

	std::string token = authService.login(email.toStdString(), password.toStdString());	

	if (token.empty()) {
		ui.statusLabel->setText("Login failed. Please check your credentials.");
		QMessageBox::warning(this, "Login Failed", "Invalid email or password.");
		return;
	}

	apiClient.setToken(token);

	std::string role;
	std::vector<std::string> permissions;

	if (!apiClient.validateToken(role, permissions)) {
		ui.statusLabel->setText("Could not validate session.");
		QMessageBox::warning(this, "Session Error", "Token validation failed.");
		return;
	}

	
	dashboardWindow = new DashboardWindow(role, permissions);
	dashboardWindow->showMaximized();

	this->hide();

	ui.statusLabel->setText("Login successful!");
}

