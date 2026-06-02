#include "SettingsPage.h"
#include "Theme.h"

#include <QMessageBox>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>

SettingsPage::SettingsPage(
    const std::string& role,
    const std::string& userName,
    QWidget* parent
)
    : QWidget(parent),
    role(role),
    userName(userName)
{
    ui.setupUi(this);

    setUserInfo(role, userName);

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
        ui.logoutButton,
        &QPushButton::clicked,
        this,
        &SettingsPage::onLogoutClicked
    );
}

SettingsPage::~SettingsPage()
{
}

void SettingsPage::setUserInfo(
    const std::string& role,
    const std::string& userName
)
{
    this->role = role;
    this->userName = userName;

    ui.nameInput->setText(QString::fromStdString(userName));
    ui.roleValueLabel->setText(QString::fromStdString(role));
}

void SettingsPage::onSaveProfileClicked()
{
    QString newName = ui.nameInput->text().trimmed();

    if (newName.isEmpty()) {
        QMessageBox::warning(
            this,
            "Invalid Name",
            "Name cannot be empty."
        );
        return;
    }

    userName = newName.toStdString();

    emit userNameChanged(userName);

    QMessageBox::information(
        this,
        "Saved",
        "Profile updated successfully."
    );
}

void SettingsPage::onApplyThemeClicked()
{
    QString selectedTheme = ui.themeCombo->currentText();

    emit themeChanged(selectedTheme);

    QMessageBox::information(
        this,
        "Theme Updated",
        "Theme changed to " + selectedTheme + "."
    );
}

void SettingsPage::onLogoutClicked()
{
    emit logoutRequested();
}

void SettingsPage::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::settingsPageStyle(theme)
    );
}

