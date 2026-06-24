// InventoryQtApp.cpp - Implementation of the login window

#include "InventoryQtApp.h"
#include "Theme.h"
#include "ErrorHandler.h"
#include "Config.h"
#include "SecureStore.h"

#include <QMessageBox>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QDebug>
#include <QFileInfo>
#include <QProcess>
#include <QApplication>
#include <QThread>
#include <QPointer>

// Constructor initializes the login window and sets up API client
InventoryQtApp::InventoryQtApp(QWidget* parent)
    :
    QMainWindow(parent),
    apiClient(Config::API_BASE_URL()),
    authService(apiClient)
{
    ui.setupUi(this);

    this->setWindowTitle("Stockvio — Sign in");
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

    // Initialize auto-update manager with backend update endpoint
    QString updateServerUrl =
        QString::fromStdString(
            Config::getUpdateServerUrl()
        );

    if (!updateServerUrl.trimmed().isEmpty()) {
        qDebug() << "Update server URL:" << updateServerUrl;

        updateManager =
            new AutoUpdateManager(
                updateServerUrl,
                this
            );

        connect(
            updateManager,
            &AutoUpdateManager::updateAvailable,
            this,
            &InventoryQtApp::onUpdateAvailable
        );

        connect(
            updateManager,
            &AutoUpdateManager::updateDownloadFinished,
            this,
            &InventoryQtApp::onUpdateDownloadFinished
        );

        connect(
            updateManager,
            &AutoUpdateManager::updateError,
            this,
            [](const QString& error) {
                qWarning() << "Update error:" << error;
            }
        );

        connect(
            updateManager,
            &AutoUpdateManager::noUpdateAvailable,
            this,
            []() {
                qDebug() << "No update available.";
            }
        );

        QTimer::singleShot(
            2000,
            this,
            [this]() {
                if (!updateManager) {
                    return;
                }

                updateManager->checkForUpdates();
            }
        );
    }
    else {
        qDebug() << "Auto-update check disabled.";
    }

    connect(
        ui.loginButton, &QPushButton::clicked,
        this, &InventoryQtApp::onLoginButtonClicked
    );

    connect(
        ui.forgotPasswordButton, &QPushButton::clicked,
        this, &InventoryQtApp::onForgotPasswordClicked
    );

    // Load saved credentials after updater is initialized
    loadSavedCredentials();
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
    ui.loginButton->setEnabled(false);

    // Run the blocking network calls on a worker thread so the UI stays
    // responsive, then finish on the UI thread in onLoginFinished().
    QPointer<InventoryQtApp> self(this);

    QThread* worker = QThread::create([self, email, password]() {
        if (!self) {
            return;
        }

        LoginResult result = self->authService.login(
            email.toStdString(),
            password.toStdString()
        );

        std::string role;
        std::vector<std::string> permissions;
        bool validated = false;

        if (result.success) {
            self->apiClient.setAccessToken(result.accessToken);
            self->apiClient.setRefreshToken(result.refreshToken);
            validated = self->apiClient.validateToken(role, permissions);
        }

        QMetaObject::invokeMethod(self, [self, result, role, permissions, validated]() {
            if (!self) {
                return;
            }

            self->onLoginFinished(result, role, permissions, validated);
        }, Qt::QueuedConnection);
    });

    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void InventoryQtApp::onLoginFinished(
    const LoginResult& result,
    const std::string& role,
    const std::vector<std::string>& permissions,
    bool validated
)
{
    ui.loginButton->setEnabled(true);

    if (!result.success) {
        QString errorMsg = QString::fromStdString(result.errorMessage);
        ui.statusLabel->setText("Login failed: " + errorMsg);

        if (errorMsg.contains("connect") || errorMsg.contains("offline")) {
            QMessageBox::critical(this, "Connection Error",
                "Unable to connect to the server.\n\n"
                "Please check:\n"
                "1. Your internet connection\n"
                "2. Firewall/network restrictions\n\n"
                "If the problem persists, contact your administrator.");
        }
        return;
    }

    if (!validated) {
        ui.statusLabel->setText("Token validation failed.");
        QMessageBox::warning(this, "Error", "Token validation failed.");
        return;
    }

    // Tokens were already applied to the API client on the worker thread.
    if (ui.rememberCheck->isChecked()) {
        saveCredentials(
            ui.emailInput->text(),
            QString::fromStdString(result.accessToken),
            QString::fromStdString(result.userName)
        );
    }
    else {
        clearSavedCredentials();
    }

    openDashboard(role, permissions, result.userName);
}

void InventoryQtApp::openDashboard(
    const std::string& role,
    const std::vector<std::string>& permissions,
    const std::string& userName
)
{
    if (dashboardWindow) {
        dashboardWindow->close();
        dashboardWindow->deleteLater();
        dashboardWindow = nullptr;
    }

    dashboardWindow = new DashboardWindow(
        role,
        permissions,
        userName.empty() ? std::string("User") : userName,
        productService,
        assetService,
        userService,
        reportService,
        truckStockService,
        aiService
    );

    connect(dashboardWindow, &DashboardWindow::logoutRequested, this, &InventoryQtApp::handleLogout);
    connect(dashboardWindow, &QObject::destroyed, this, [this]() {
        dashboardWindow = nullptr;
        });

    dashboardWindow->showMaximized();
    this->hide();
}

void InventoryQtApp::handleLogout()
{
    clearSavedCredentials();
    authService.logout();

    if (dashboardWindow) {
        DashboardWindow* windowToClose = dashboardWindow;
        dashboardWindow = nullptr;
        windowToClose->close();
        windowToClose->deleteLater();
    }

    ui.statusLabel->setText("");
    ui.passwordInput->clear();
    ui.rememberCheck->setChecked(false);

    // Re-apply in case the user changed the appearance preference during the
    // session, so the login screen returns in the currently selected theme.
    Theme::applyLogin(this);

    this->show();
    this->raise();
    this->activateWindow();
}

void InventoryQtApp::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::dataPageStyle(theme)
    );
}

void InventoryQtApp::saveCredentials(const QString& email, const QString& accessToken, const QString& userName)
{
    QSettings settings("InventorySystem", "InventoryQtApp");

    settings.setValue("login/email", email);
    settings.setValue("login/accessToken", SecureStore::encrypt(accessToken));
    settings.setValue("login/userName", userName.isEmpty() ? email : userName);

    if (!apiClient.getRefreshToken().empty()) {
        settings.setValue("login/refreshToken",
            SecureStore::encrypt(QString::fromStdString(apiClient.getRefreshToken())));
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

        // Stored tokens are DPAPI-encrypted. If decryption fails the value is
        // legacy plaintext from an older build, so fall back to it and let the
        // next save re-encrypt it.
        QString rawAccessToken = settings.value("login/accessToken", "").toString();
        QString rawRefreshToken = settings.value("login/refreshToken", "").toString();

        QString savedAccessToken = SecureStore::decrypt(rawAccessToken);
        if (savedAccessToken.isEmpty()) {
            savedAccessToken = rawAccessToken;
        }

        QString savedRefreshToken = SecureStore::decrypt(rawRefreshToken);
        if (savedRefreshToken.isEmpty()) {
            savedRefreshToken = rawRefreshToken;
        }

        if (!savedEmail.isEmpty() &&
            (!savedAccessToken.isEmpty() || !savedRefreshToken.isEmpty())) {

            ui.emailInput->setText(savedEmail);
            ui.rememberCheck->setChecked(true);

            qDebug() << "Loading saved credentials for:" << savedEmail;

            if (!savedAccessToken.isEmpty()) {
                apiClient.setAccessToken(savedAccessToken.toStdString());
            }

            if (!savedRefreshToken.isEmpty()) {
                apiClient.setRefreshToken(savedRefreshToken.toStdString());
            }

            std::string role;
            std::vector<std::string> permissions;

            if (apiClient.validateToken(role, permissions)) {
                ui.statusLabel->setText("Resuming session...");

                QString userName = settings.value("login/userName", "User").toString();

                openDashboard(role, permissions, userName.toStdString());
            }
            else {
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

void InventoryQtApp::onForgotPasswordClicked()
{
    QString email = ui.emailInput->text();

    if (email.isEmpty()) {
        QMessageBox::warning(this, "Forgot Password", "Please enter your email address.");
        return;
    }

    auto result = authService.requestPasswordReset(email.toStdString());

    if (result.success) {
        QMessageBox::information(this, "Password Reset",
            "Password reset instructions have been sent to:\n" + email + "\n\n"
            "Please check your email to reset your password.");

        ui.statusLabel->setText("Check your email for password reset instructions.");
    }
    else {
        QMessageBox::critical(this, "Password Reset Failed",
            "Failed to request password reset:\n" +
            QString::fromStdString(result.errorMessage),
            QMessageBox::Ok);

        ui.statusLabel->setText("Password reset request failed. Please try again.");
    }
}

void InventoryQtApp::onUpdateAvailable(const UpdateInfo& info)
{
    int reply = QMessageBox::question(this, "Update Available",
        "A new version of Stockvio is available.\n\n"
        "Version: " + info.version + "\n\n"
        "Release Notes:\n" + info.releaseNotes + "\n\n"
        "Would you like to download and install the update now?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        updateManager->downloadAndInstallUpdate(info);

        QMessageBox::information(this, "Downloading Update",
            "The update is being downloaded in the background.\n"
            "You will be prompted to install it once the download is complete.");
    }
}

void InventoryQtApp::onUpdateDownloadFinished(const QString& filePath)
{
    QFileInfo info(filePath);

    if (!info.exists() || info.size() == 0) {
        QMessageBox::warning(this, "Update",
            "The update download did not complete successfully. Please try again later.");
        return;
    }

    int reply = QMessageBox::question(this, "Install Update",
        "The update has been downloaded.\n\n"
        "The application needs to close to complete the installation. Install now?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) {
        return;
    }

    if (QProcess::startDetached(filePath, QStringList())) {
        QApplication::quit();
    }
    else {
        QMessageBox::warning(this, "Update",
            "Could not launch the installer automatically.\n\n"
            "You can run it manually from:\n" + filePath);
    }
}