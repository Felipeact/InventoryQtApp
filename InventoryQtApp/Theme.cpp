#include "Theme.h"

#include <QApplication>

void Theme::applyApplication()
{
    applyTheme(AppTheme::Dark);
}

void Theme::applyApplication(QApplication& app)
{
    Q_UNUSED(app);
    applyTheme(AppTheme::Dark);
}

void Theme::applyLogin(QWidget* widget)
{
    if (!widget) {
        return;
    }

    widget->setStyleSheet(darkStyleSheet());
}

void Theme::applyTheme(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        qApp->setStyleSheet(lightStyleSheet());
    }
    else {
        qApp->setStyleSheet(darkStyleSheet());
    }
}

QString Theme::globalStyleSheet()
{
    return darkStyleSheet();
}

QString Theme::darkStyleSheet()
{
    return R"(
* {
    font-family: "Segoe UI";
    font-size: 13px;
}

QMainWindow,
QWidget {
    background-color: #050B14;
    color: #FFFFFF;
}

QLabel {
    color: #FFFFFF;
    background: transparent;
}

QLineEdit,
QComboBox,
QSpinBox,
QTextEdit,
QPlainTextEdit {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 6px;
    color: #FFFFFF;
    padding: 8px 10px;
    selection-background-color: #2563EB;
}

QPushButton {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 6px;
    color: #FFFFFF;
    padding: 8px 12px;
    font-weight: 700;
}

QPushButton:hover {
    background-color: #172033;
}

QPushButton:checked {
    background-color: #1D4ED8;
    color: #FFFFFF;
}

QTableWidget {
    background-color: #0B1626;
    border: 1px solid #263A57;
    color: #FFFFFF;
    gridline-color: #1F2937;
}

QHeaderView::section {
    background-color: #111827;
    color: #FFFFFF;
    border: none;
    padding: 8px;
    font-weight: 700;
}

QFrame {
    background: transparent;
}
)";
}

QString Theme::lightStyleSheet()
{
    return R"(
* {
    font-family: "Segoe UI";
    font-size: 13px;
}

QMainWindow,
QWidget {
    background-color: #F8FAFC;
    color: #111827;
}

QLabel {
    color: #111827;
    background: transparent;
}

QLineEdit,
QComboBox,
QSpinBox,
QTextEdit,
QPlainTextEdit {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 6px;
    color: #111827;
    padding: 8px 10px;
    selection-background-color: #2563EB;
}

QPushButton {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 6px;
    color: #111827;
    padding: 8px 12px;
    font-weight: 700;
}

QPushButton:hover {
    background-color: #EFF6FF;
    border-color: #2563EB;
}

QPushButton:checked {
    background-color: #DBEAFE;
    color: #1D4ED8;
}

QTableWidget {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    color: #111827;
    gridline-color: #E5E7EB;
}

QHeaderView::section {
    background-color: #E2E8F0;
    color: #111827;
    border: none;
    padding: 8px;
    font-weight: 700;
}

QFrame {
    background: transparent;
}
)";
}

QString Theme::sidebarStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#SidebarWidgetClass { background-color: #FFFFFF; border-right: 1px solid #E2E8F0; }
QLabel#logoLabel { font-size: 18px; font-weight: 800; color: #111827; }
QLabel#sectionLabel, QLabel#sectionLabel2 { color: #64748B; font-size: 11px; font-weight: 700; letter-spacing: 1px; }
QFrame#lineDivider, QFrame#lineDivider2 { color: #E2E8F0; }
QPushButton { text-align: left; padding: 10px 12px; border-radius: 8px; border: none; background: transparent; color: #111827; font-weight: 600; }
QPushButton:hover { background-color: #EFF6FF; }
QPushButton:checked { background-color: #DBEAFE; color: #1D4ED8; }
QFrame#userCard { background-color: #F8FAFC; border: 1px solid #E2E8F0; border-radius: 10px; }
QLabel#avatarLabel { background-color: #2563EB; border-radius: 14px; color: white; font-weight: 800; }
QLabel#userNameLabel { font-size: 12px; font-weight: 700; color: #111827; }
QLabel#userRoleLabel { font-size: 10px; color: #64748B; }
QPushButton#logoutButton { background-color: #DC2626; color: white; }
)";
    }

    return R"(
QWidget#SidebarWidgetClass { background-color: #08111F; border-right: 1px solid #1E293B; }
QLabel#logoLabel { font-size: 18px; font-weight: 800; color: white; }
QLabel#sectionLabel, QLabel#sectionLabel2 { color: #94A3B8; font-size: 11px; font-weight: 700; }
QFrame#lineDivider, QFrame#lineDivider2 { color: #1E293B; }
QPushButton { text-align: left; padding: 10px 12px; border-radius: 8px; border: none; background: transparent; color: white; font-weight: 600; }
QPushButton:hover { background-color: #172033; }
QPushButton:checked { background-color: #1D4ED8; }
QFrame#userCard { background-color: #0B1626; border: 1px solid #263A57; border-radius: 10px; }
QLabel#avatarLabel { background-color: #1D4ED8; border-radius: 14px; color: white; font-weight: 800; }
QLabel#userNameLabel { font-size: 12px; font-weight: 700; color: white; }
QLabel#userRoleLabel { font-size: 10px; color: #94A3B8; }
QPushButton#logoutButton { background-color: #DC2626; color: white; }
)";
}

QString Theme::verticalBarStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#VerticalWidget { background-color: #F8FAFC; }
QLabel#welcomeLabel { color: #111827; font-size: 22px; font-weight: 700; }
QLabel#subTitleLabel { color: #64748B; font-size: 12px; }
QFrame#searchContainer { background-color: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 10px; }
QLineEdit#searchInput { background: transparent; border: none; color: #111827; padding: 0; }
QLabel#searchIcon { color: #64748B; font-size: 14px; }
QLabel#shortcutLabel { background-color: #E2E8F0; color: #475569; border-radius: 6px; padding: 4px 8px; font-size: 11px; }
QPushButton#notificationButton { background-color: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 17px; }
QLabel#avatarLabel { background-color: #2563EB; color: #FFFFFF; border-radius: 17px; font-weight: 800; }
)";
    }

    return R"(
QWidget#VerticalWidget { background-color: #050B14; }
QLabel#welcomeLabel { color: #FFFFFF; font-size: 22px; font-weight: 700; }
QLabel#subTitleLabel { color: #94A3B8; font-size: 12px; }
QFrame#searchContainer { background-color: #0B1626; border: 1px solid #263A57; border-radius: 10px; }
QLineEdit#searchInput { background: transparent; border: none; color: #FFFFFF; padding: 0; }
QLabel#searchIcon { color: #94A3B8; font-size: 14px; }
QLabel#shortcutLabel { background-color: #172033; color: #94A3B8; border-radius: 6px; padding: 4px 8px; font-size: 11px; }
QPushButton#notificationButton { background-color: #0B1626; border: 1px solid #263A57; border-radius: 17px; }
QLabel#avatarLabel { background-color: #1D4ED8; color: #FFFFFF; border-radius: 17px; font-weight: 800; }
)";
}

QString Theme::settingsPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#SettingsPageClass { background-color: #F8FAFC; }
QWidget#profileCard, QWidget#appearanceCard, QWidget#sessionCard { background-color: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 12px; }
QLabel#titleLabel { color: #111827; font-size: 22px; font-weight: 800; }
QLabel#subtitleLabel { color: #64748B; font-size: 13px; }
QLabel#profileTitleLabel, QLabel#appearanceTitleLabel, QLabel#sessionTitleLabel { color: #111827; font-size: 16px; font-weight: 800; }
QLabel#roleValueLabel { color: #2563EB; font-weight: 700; }
QPushButton#saveProfileButton, QPushButton#applyThemeButton { background-color: #2563EB; color: #FFFFFF; border: none; }
QPushButton#logoutButton { background-color: #DC2626; color: #FFFFFF; border: none; }
)";
    }

    return R"(
QWidget#SettingsPageClass { background-color: #050B14; }
QWidget#profileCard, QWidget#appearanceCard, QWidget#sessionCard { background-color: #0B1626; border: 1px solid #263A57; border-radius: 12px; }
QLabel#titleLabel { color: #FFFFFF; font-size: 22px; font-weight: 800; }
QLabel#subtitleLabel { color: #94A3B8; font-size: 13px; }
QLabel#profileTitleLabel, QLabel#appearanceTitleLabel, QLabel#sessionTitleLabel { color: #FFFFFF; font-size: 16px; font-weight: 800; }
QLabel#roleValueLabel { color: #93C5FD; font-weight: 700; }
QPushButton#saveProfileButton, QPushButton#applyThemeButton { background-color: #2563EB; color: #FFFFFF; border: none; }
QPushButton#logoutButton { background-color: #DC2626; color: #FFFFFF; border: none; }
)";
}

QString Theme::scanPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#ScanPageClass { background-color: #F8FAFC; }
QWidget#pageCard, QWidget#scanCard, QWidget#recentScansCard { background-color: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 12px; }
QLabel#pageTitleLabel, QLabel#titleLabel, QLabel#recentTitleLabel { color: #111827; font-weight: 800; }
QLabel#breadcrumbLabel, QLabel#subtitleLabel { color: #64748B; }
QLabel#quantityLabel { color: #334155; font-weight: 700; }
QLineEdit#barcodeInput { border: 1px solid #2563EB; min-height: 42px; }
QSpinBox#quantityInput { min-height: 42px; }
QPushButton#submitButton { background-color: #16A34A; color: #FFFFFF; border: none; min-height: 42px; }
QPushButton#minusButton, QPushButton#plusButton { background-color: #FFFFFF; border: 1px solid #CBD5E1; min-height: 42px; }
QLabel#statusLabel { color: #CA8A04; font-weight: 700; }
QLabel#recentCodeLabel, QLabel#recentNameLabel, QLabel#recentDateLabel, QLabel#recentCodeLabel_2, QLabel#recentNameLabel_2, QLabel#recentDateLabel_2, QLabel#recentCodeLabel_3, QLabel#recentNameLabel_3, QLabel#recentDateLabel_3 { color: #475569; font-size: 11px; }
QLabel#recentQtyLabel, QLabel#recentQtyLabel_2, QLabel#recentQtyLabel_3 { color: #16A34A; font-weight: 800; }
)";
    }

    return R"(
QWidget#ScanPageClass { background-color: #050B14; }
QWidget#pageCard { background-color: #0B1626; border: 1px solid #263A57; border-radius: 12px; }
QWidget#scanCard, QWidget#recentScansCard { background-color: #0D1828; border: 1px solid #1E2F46; border-radius: 10px; }
QLabel#pageTitleLabel, QLabel#titleLabel, QLabel#recentTitleLabel { color: #FFFFFF; font-weight: 800; }
QLabel#breadcrumbLabel, QLabel#subtitleLabel { color: #94A3B8; }
QLabel#quantityLabel { color: #E5EEF8; font-weight: 700; }
QLineEdit#barcodeInput { border: 1px solid #3B82F6; min-height: 42px; }
QSpinBox#quantityInput { min-height: 42px; }
QPushButton#submitButton { background-color: #16A34A; color: #FFFFFF; border: none; min-height: 42px; }
QPushButton#minusButton, QPushButton#plusButton { background-color: #0B1626; border: 1px solid #263A57; min-height: 42px; }
QLabel#statusLabel { color: #FACC15; font-weight: 700; }
QLabel#recentCodeLabel, QLabel#recentNameLabel, QLabel#recentDateLabel, QLabel#recentCodeLabel_2, QLabel#recentNameLabel_2, QLabel#recentDateLabel_2, QLabel#recentCodeLabel_3, QLabel#recentNameLabel_3, QLabel#recentDateLabel_3 { color: #CBD5E1; font-size: 11px; }
QLabel#recentQtyLabel, QLabel#recentQtyLabel_2, QLabel#recentQtyLabel_3 { color: #22C55E; font-weight: 800; }
)";
}

QString Theme::dashboardPageStyle(AppTheme theme)
{
    return dataPageStyle(theme);
}

QString Theme::dataPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget { background-color: #F8FAFC; color: #111827; }
QFrame, QWidget#pageCard, QWidget#card, QWidget#tableCard, QWidget#toolbarCard { background-color: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 12px; }
QLabel { color: #111827; }
)";
    }

    return R"(
QWidget { background-color: #050B14; color: #FFFFFF; }
QFrame, QWidget#pageCard, QWidget#card, QWidget#tableCard, QWidget#toolbarCard { background-color: #0B1626; border: 1px solid #263A57; border-radius: 12px; }
QLabel { color: #FFFFFF; }
)";
}

QString Theme::dialogStyle(AppTheme theme)
{
    return dataPageStyle(theme);
}

QString Theme::truckPageStyle(AppTheme theme)
{
    return dataPageStyle(theme);
}
