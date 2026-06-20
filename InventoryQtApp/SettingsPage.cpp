#include "SettingsPage.h"
#include "Theme.h"
#include "Config.h"

#include <QMessageBox>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QSettings>
#include <QUrl>

SettingsPage::SettingsPage(
    const std::string& role,
    const std::string& userName,
    UserService* userService,
    QWidget* parent
)
    : QWidget(parent),
    role(role),
    userName(userName),
    userService(userService)
{
    ui.setupUi(this);

    // The backend API endpoint is an internal configuration detail. Users must not
    // see or change it, so keep the API Connection card out of the Settings UI.
    ui.apiCard->hide();

    setUserInfo(
        role,
        userName
    );

    loadSettings();

    connect(
        ui.saveProfileButton,
        &QPushButton::clicked,
        this,
        &SettingsPage::onSaveProfileClicked
    );

    connect(
        ui.applyThemeButton,
        &QPushButton::clicked,
        this,
        &SettingsPage::onApplyThemeClicked
    );

    connect(
        ui.saveApiUrlButton,
        &QPushButton::clicked,
        this,
        &SettingsPage::onSaveApiUrlClicked
    );

    connect(
        ui.resetApiUrlButton,
        &QPushButton::clicked,
        this,
        &SettingsPage::onResetApiUrlClicked
    );

    connect(
        ui.logoutButton,
        &QPushButton::clicked,
        this,
        &SettingsPage::onLogoutClicked
    );
}

SettingsPage::~SettingsPage()
{
}

void SettingsPage::loadSettings()
{
    QSettings settings(
        "InventorySystem",
        "InventoryQtApp"
    );

    QString savedTheme =
        settings.value(
            "appearance/theme",
            "Light"
        ).toString();

    int themeIndex =
        ui.themeCombo->findText(savedTheme);

    if (themeIndex >= 0) {
        ui.themeCombo->setCurrentIndex(themeIndex);
    }

    ui.apiUrlInput->setText(
        QString::fromStdString(
            Config::getApiBaseUrl()
        )
    );
}

void SettingsPage::setUserInfo(
    const std::string& role,
    const std::string& userName
)
{
    this->role = role;
    this->userName = userName;

    ui.nameInput->setText(
        QString::fromStdString(userName)
    );

    ui.roleValueLabel->setText(
        QString::fromStdString(role)
    );
}

void SettingsPage::onSaveProfileClicked()
{
    QString newName =
        ui.nameInput->text().trimmed();

    if (newName.isEmpty()) {
        QMessageBox::warning(
            this,
            "Invalid Name",
            "Name cannot be empty."
        );

        return;
    }

    if (!userService) {
        QMessageBox::warning(
            this,
            "Profile Update Failed",
            "User service is unavailable."
        );

        return;
    }

    bool success =
        userService->updateCurrentUserProfile(
            newName.toStdString()
        );

    if (!success) {
        QMessageBox::warning(
            this,
            "Profile Update Failed",
            "Could not update your profile in the backend/database."
        );

        return;
    }

    userName =
        newName.toStdString();

    QSettings settings(
        "InventorySystem",
        "InventoryQtApp"
    );

    settings.setValue(
        "profile/displayName",
        newName
    );

    emit userNameChanged(userName);

    QMessageBox::information(
        this,
        "Saved",
        "Profile updated successfully."
    );
}

void SettingsPage::onApplyThemeClicked()
{
    QString selectedTheme =
        ui.themeCombo->currentText();

    QSettings settings(
        "InventorySystem",
        "InventoryQtApp"
    );

    settings.setValue(
        "appearance/theme",
        selectedTheme
    );

    emit themeChanged(selectedTheme);

    QMessageBox::information(
        this,
        "Theme Updated",
        "Theme changed to " + selectedTheme + "."
    );
}

void SettingsPage::onSaveApiUrlClicked()
{
    QString apiUrl =
        ui.apiUrlInput->text().trimmed();

    if (!isValidApiUrl(apiUrl)) {
        QMessageBox::warning(
            this,
            "Invalid API URL",
            "Please enter a valid URL starting with http:// or https://."
        );

        return;
    }

    Config::setApiBaseUrl(
        apiUrl.toStdString()
    );

    QMessageBox::information(
        this,
        "API URL Saved",
        "API URL saved successfully.\n\nRestart the application for all services to use the new URL."
    );
}

void SettingsPage::onResetApiUrlClicked()
{
    QMessageBox::StandardButton confirm =
        QMessageBox::question(
            this,
            "Reset API URL",
            "Reset API URL to the default value?",
            QMessageBox::Yes | QMessageBox::No
        );

    if (confirm != QMessageBox::Yes) {
        return;
    }

    Config::resetApiBaseUrl();

    ui.apiUrlInput->setText(
        QString::fromStdString(
            Config::DEFAULT_API_BASE_URL
        )
    );

    QMessageBox::information(
        this,
        "API URL Reset",
        "API URL reset to default.\n\nRestart the application for all services to use the default URL."
    );
}

void SettingsPage::onLogoutClicked()
{
    emit logoutRequested();
}

bool SettingsPage::isValidApiUrl(
    const QString& url
) const
{
    QUrl parsedUrl(url);

    return parsedUrl.isValid() &&
        (
            parsedUrl.scheme() == "http" ||
            parsedUrl.scheme() == "https"
            ) &&
        !parsedUrl.host().isEmpty();
}

void SettingsPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::settingsPageStyle(theme)
    );
}