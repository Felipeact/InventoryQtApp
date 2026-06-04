// InventoryQtApp.cpp - Implementation of the login window
#include "InventoryQtApp.h"
#include "Theme.h"
#include "ErrorHandler.h"
#include <QMessageBox>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>

#include "Config.h"

// Constructor initializes the login window and sets up API client
InventoryQtApp::InventoryQtApp(QWidget* parent)
	:
	QMainWindow(parent),
	apiClient(Config::API_BASE_URL()),
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

	// Load saved credentials if available
	loadSavedCredentials();

	// Initialize auto-update manager with update server URL
	// TODO: Change this to your production update server URL
	QString updateServerUrl = "https://updates.your-company.com/api/app/latest-version";
	updateManager = new AutoUpdateManager(updateServerUrl, this);
	connect(updateManager, &AutoUpdateManager::updateAvailable,
			this, &InventoryQtApp::onUpdateAvailable);
	connect(updateManager, &AutoUpdateManager::updateError,
			this, [this](const QString& error) {
				qWarning() << "Update error:" << error;
			});

	// Check for updates on startup (after a short delay)
	QTimer::singleShot(2000, this, [this]() {
		updateManager->checkForUpdates();
	});

	connect(
		ui.loginButton, &QPushButton::clicked,
		this, &InventoryQtApp::onLoginButtonClicked
	);

	connect(
		ui.forgotPasswordButton, &QPushButton::clicked,
		this, &InventoryQtApp::onForgotPasswordClicked
	);
}

// Destructor
InventoryQtApp::~InventoryQtApp()
{
}

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
		// Display user-friendly error message
		QString errorMsg = QString::fromStdString(loginResult.errorMessage);
		ui.statusLabel->setText("Login failed: " + errorMsg);

		// Show detailed message for offline scenarios
		if (errorMsg.contains("connect") || errorMsg.contains("offline")) {
			QMessageBox::critical(this, "Connection Error",
				"Unable to connect to the server.\n\n"
				"Please check:\n"
				"1. Your internet connection\n"
				"2. The API server address in settings\n"
				"3. Firewall/network restrictions");
		}
		return;
	}

	// Save credentials if Remember Me is checked
	if (ui.rememberCheck->isChecked()) {
		// Set both tokens before saving
		apiClient.setAccessToken(loginResult.accessToken);
		apiClient.setRefreshToken(loginResult.refreshToken);
		saveCredentials(email, QString::fromStdString(loginResult.accessToken));
	} else {
		clearSavedCredentials();
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

	dashboardWindow = new DashboardWindow(role, permissions, loginResult.userName, productService, assetService, userService, reportService, truckStockService);

	connect(dashboardWindow, &DashboardWindow::logoutRequested, this, [this]() {
		authService.logout();
		ui.statusLabel->setText("");
		this->show();
		});

	dashboardWindow->showMaximized();
	this->hide();
}

void InventoryQtApp::applyTheme(Theme::AppTheme theme)
{
	setStyleSheet(
		Theme::dataPageStyle(theme)
	);
}

void InventoryQtApp::saveCredentials(const QString& email, const QString& accessToken)
{
	QSettings settings("InventorySystem", "InventoryQtApp");

	// Save email (not sensitive)
	settings.setValue("login/email", email);

	// TODO: Implement secure token storage (e.g., using DPAPI on Windows or OS keychain)
	// For now, use basic storage - in production, encrypt tokens
	// Current approach: Store in registry with group encryption on Windows
	settings.setValue("login/accessToken", accessToken);

	// Also save refresh token if available
	if (!apiClient.getRefreshToken().empty()) {
		settings.setValue("login/refreshToken", QString::fromStdString(apiClient.getRefreshToken()));
	}

	settings.setValue("login/rememberMe", true);
	settings.sync();

	qDebug() << "Credentials saved for:" << email;
}

void InventoryQtApp::loadSavedCredentials()
{
	QSettings settings("InventorySystem", "InventoryQtApp");

	bool rememberMe = settings.value("login/rememberMe", false).toBool();
	if (rememberMe) {
		QString savedEmail = settings.value("login/email", "").toString();
		QString savedAccessToken = settings.value("login/accessToken", "").toString();
		QString savedRefreshToken = settings.value("login/refreshToken", "").toString();

		if (!savedEmail.isEmpty() && (!savedAccessToken.isEmpty() || !savedRefreshToken.isEmpty())) {
			ui.emailInput->setText(savedEmail);
			ui.rememberCheck->setChecked(true);

			qDebug() << "Loading saved credentials for:" << savedEmail;

			// Try access token first, then refresh token
			if (!savedAccessToken.isEmpty()) {
				apiClient.setAccessToken(savedAccessToken.toStdString());
			}

			if (!savedRefreshToken.isEmpty()) {
				apiClient.setRefreshToken(savedRefreshToken.toStdString());
			}

			// Validate token
			std::string role;
			std::vector<std::string> permissions;

			if (apiClient.validateToken(role, permissions)) {
				// Token is valid, proceed to dashboard
				ui.statusLabel->setText("Resuming session...");

				// Get user name from token or use stored value
				QString userName = settings.value("login/userName", "User").toString();

				dashboardWindow = new DashboardWindow(role, permissions, userName.toStdString(), productService, assetService, userService, reportService, truckStockService);

				connect(dashboardWindow, &DashboardWindow::logoutRequested, this, [this]() {
					clearSavedCredentials();
					authService.logout();
					ui.statusLabel->setText("");
					ui.emailInput->clear();
					ui.passwordInput->clear();
					this->show();
				});

				dashboardWindow->showMaximized();
				this->hide();
			} else {
				// Token expired, clear credentials and prompt login
				qWarning() << "Saved token validation failed";
				clearSavedCredentials();
				ui.statusLabel->setText("Session expired. Please login again.");
			}
		}
	}
}

void InventoryQtApp::clearSavedCredentials()
{
	QSettings settings("InventorySystem", "InventoryQtApp");
	settings.remove("login/email");
	settings.remove("login/accessToken");
	settings.remove("login/refreshToken");
	settings.remove("login/userName");
	settings.setValue("login/rememberMe", false);
	settings.sync();

	qDebug() << "Credentials cleared";
}

QString InventoryQtApp::getEncryptedToken() const
{
	QSettings settings("InventorySystem", "InventoryQtApp");
	return settings.value("login/accessToken", "").toString();
}

void InventoryQtApp::onForgotPasswordClicked()
{
	QString email = ui.emailInput->text();

	if (email.isEmpty()) {
		QMessageBox::warning(this, "Forgot Password", "Please enter your email address.");
		return;
	}

	// Call backend to request password reset
	auto result = authService.requestPasswordReset(email.toStdString());

	if (result.success) {
		QMessageBox::information(this, "Password Reset",
			"Password reset instructions have been sent to:\n" + email + "\n\n"
			"Please check your email to reset your password.");
		ui.statusLabel->setText("Check your email for password reset instructions.");
	} else {
		QMessageBox::critical(this, "Password Reset Failed",
			"Failed to request password reset:\n" + QString::fromStdString(result.errorMessage),
			QMessageBox::Ok);
		ui.statusLabel->setText("Password reset request failed. Please try again.");
	}
}

void InventoryQtApp::onUpdateAvailable(const UpdateInfo& info)
{
	int reply = QMessageBox::question(this, "Update Available",
		"A new version of Inventory Management System is available.\n\n"
		"Version: " + info.version + "\n\n"
		"Release Notes:\n" + info.releaseNotes + "\n\n"
		"Would you like to download and install the update now?",
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		updateManager->downloadAndInstallUpdate(info);
		QMessageBox::information(this, "Downloading Update",
			"The update will be downloaded in the background.\n"
			"The application will restart once the download is complete.");
	}
}

