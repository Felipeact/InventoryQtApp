#include "Theme.h"

#include <QApplication>
#include <QSettings>
#include <QString>

void Theme::applyApplication(QApplication& app)
{
    Q_UNUSED(app);
    applyTheme(AppTheme::Light);
}

void Theme::applyLogin(QWidget* widget)
{
    if (!widget) {
        return;
    }

    // Honor the user's saved appearance preference so the login screen matches
    // the rest of the app (the dashboard reads the same key). Defaults to Light.
    QSettings settings("InventorySystem", "InventoryQtApp");
    const QString savedTheme =
        settings.value("appearance/theme", "Light").toString();
    const AppTheme theme =
        savedTheme == "Dark" ? AppTheme::Dark : AppTheme::Light;

    widget->setStyleSheet(loginStyle(theme));
}

void Theme::applyTheme(AppTheme theme)
{
    qApp->setStyleSheet(theme == AppTheme::Light ? lightStyleSheet() : darkStyleSheet());
}

QString Theme::globalStyleSheet()
{
    return lightStyleSheet();
}

QString Theme::darkStyleSheet()
{
    return R"(
* { font-family: "Segoe UI"; font-size: 13px; }

QMainWindow, QWidget {
    background-color: #0A1A2B;
    color: #F3F5F9;
}

QLabel {
    color: #F3F5F9;
    background: transparent;
}

QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QTextEdit, QPlainTextEdit, QDateEdit {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 12px;
    color: #F3F5F9;
    padding: 10px 14px;
    selection-background-color: #1D4E89;
    selection-color: #FFFFFF;
}

QLineEdit:hover, QComboBox:hover, QSpinBox:hover, QDoubleSpinBox:hover,
QTextEdit:hover, QPlainTextEdit:hover, QDateEdit:hover {
    border-color: #2E5273;
}

QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus,
QTextEdit:focus, QPlainTextEdit:focus, QDateEdit:focus {
    border: 1px solid #2C6BB3;
    background-color: #0F2A42;
}

QComboBox::drop-down { border: none; width: 22px; }
QComboBox::down-arrow {
    image: none;
    width: 0px; height: 0px;
    border-left: 4px solid transparent;
    border-right: 4px solid transparent;
    border-top: 5px solid #90A0B3;
    margin-right: 10px;
}
QComboBox QAbstractItemView {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    color: #F3F5F9;
    padding: 4px;
    outline: 0;
    selection-background-color: #1D4E89;
    selection-color: #FFFFFF;
}

QPushButton {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #16314A, stop:1 #0F2438);
    border: 1px solid #1E3A52;
    border-radius: 12px;
    color: #F3F5F9;
    padding: 10px 18px;
    font-weight: 700;
}

QPushButton:hover {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #274463, stop:1 #16314A);
    border-color: #2C6BB3;
}

QPushButton:pressed {
    background-color: #102A41;
}

QPushButton:checked {
    background-color: #1D4E89;
    border-color: #1D4E89;
    color: #FFFFFF;
}

QPushButton:disabled {
    color: #6B7C93;
    background-color: #0A1A2B;
    border-color: #274463;
}

/* Harbor button variants - set via the dynamic `variant` property in .ui */
QPushButton[variant="primary"] { background-color: #2C6BB3; border: none; color: #FFFFFF; font-weight: 700; }
QPushButton[variant="primary"]:hover { background-color: #3D7CC0; }
QPushButton[variant="accent"] { background-color: #F5A623; border: none; color: #11314F; font-weight: 800; }
QPushButton[variant="accent"]:hover { background-color: #FBBF24; }
QPushButton[variant="danger"] { background-color: #D7263D; border: none; color: #FFFFFF; font-weight: 700; }
QPushButton[variant="danger"]:hover { background-color: #E5384B; }
QPushButton[variant="ghost"] { background: transparent; border: 1px solid #1E3A52; color: #7FAAD6; font-weight: 700; }
QPushButton[variant="ghost"]:hover { background-color: #16314A; border-color: #274463; }

QTableWidget, QTreeWidget, QListWidget {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 16px;
    color: #F3F5F9;
    gridline-color: #16314A;
    selection-background-color: #1D4E89;
    selection-color: #FFFFFF;
    alternate-background-color: #102A41;
    outline: 0;
    padding: 6px;
}

QTableWidget::item, QTreeWidget::item, QListWidget::item {
    padding: 12px 14px;
    border: none;
    border-bottom: 1px solid #1B2740;
}
QTableWidget::item:hover, QTreeWidget::item:hover, QListWidget::item:hover {
    background-color: #16314A;
}
QTableWidget::item:selected, QTreeWidget::item:selected, QListWidget::item:selected {
    background-color: #1D4E89;
    color: #FFFFFF;
}

QHeaderView::section {
    background-color: #102A41;
    color: #90A0B3;
    border: none;
    border-bottom: 2px solid #274463;
    padding: 13px 14px;
    font-weight: 700;
}
QTableCornerButton::section { background-color: #102A41; border: none; }

QFrame { background: transparent; }

QScrollBar:vertical {
    background: transparent;
    width: 10px;
    margin: 4px;
}
QScrollBar::handle:vertical {
    background: #1E3A52;
    border-radius: 5px;
    min-height: 36px;
}
QScrollBar::handle:vertical:hover { background: #1D4E89; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }
QScrollBar:horizontal {
    background: transparent;
    height: 10px;
    margin: 4px;
}
QScrollBar::handle:horizontal {
    background: #1E3A52;
    border-radius: 5px;
    min-width: 36px;
}
QScrollBar::handle:horizontal:hover { background: #1D4E89; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }

QCheckBox, QRadioButton { color: #C2CEDB; spacing: 8px; }

QMenu {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 10px;
    padding: 6px;
}
QMenu::item { padding: 7px 18px; border-radius: 6px; color: #F3F5F9; }
QMenu::item:selected { background-color: #1D4E89; color: #FFFFFF; }

QTabWidget::pane { border: 1px solid #1E3A52; border-radius: 10px; top: -1px; }
QTabBar::tab {
    background: transparent;
    color: #90A0B3;
    padding: 9px 16px;
    border: none;
    border-bottom: 2px solid transparent;
}
QTabBar::tab:selected { color: #F3F5F9; border-bottom: 2px solid #1D4E89; }
QTabBar::tab:hover { color: #C2CEDB; }

QProgressBar {
    background-color: #0F2438;
    border: none;
    border-radius: 6px;
    text-align: center;
    color: transparent;
}
QProgressBar::chunk {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2C6BB3, stop:1 #1D4E89);
    border-radius: 6px;
}

QToolTip {
    background-color: #102A41;
    color: #F3F5F9;
    border: 1px solid #1D4E89;
    border-radius: 6px;
    padding: 6px 8px;
}
)";
}

QString Theme::lightStyleSheet()
{
    return R"(
* { font-family: "Segoe UI"; font-size: 13px; }

QMainWindow, QWidget {
    background-color: #F3F5F9;
    color: #0F2942;
}

QLabel {
    color: #1B3A57;
    background: transparent;
}

QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QTextEdit, QPlainTextEdit, QDateEdit {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 10px;
    color: #0F2942;
    padding: 10px 14px;
    selection-background-color: #1D4E89;
    selection-color: #FFFFFF;
}

QLineEdit:hover, QComboBox:hover, QSpinBox:hover, QDoubleSpinBox:hover,
QTextEdit:hover, QPlainTextEdit:hover, QDateEdit:hover {
    border-color: #C2CEDB;
}

QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus,
QTextEdit:focus, QPlainTextEdit:focus, QDateEdit:focus {
    border: 2px solid #F59E0B;
    padding: 8px 11px;
}

QComboBox::drop-down { border: none; width: 22px; }
QComboBox::down-arrow {
    image: none;
    width: 0px; height: 0px;
    border-left: 4px solid transparent;
    border-right: 4px solid transparent;
    border-top: 5px solid #90A0B3;
    margin-right: 10px;
}
QComboBox QAbstractItemView {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
    color: #1B3A57;
    padding: 4px;
    outline: 0;
    selection-background-color: #1D4E89;
    selection-color: #FFFFFF;
}

QPushButton {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 10px;
    color: #33475B;
    padding: 10px 18px;
    font-weight: 600;
}

QPushButton:hover {
    background-color: #EEF3F9;
    border-color: #BCD3EA;
    color: #163E6C;
}

QPushButton:pressed {
    background-color: #E7EFF8;
}

QPushButton:checked {
    background-color: #1D4E89;
    border-color: #1D4E89;
    color: #FFFFFF;
}

QPushButton:disabled {
    color: #90A0B3;
    background-color: #EDF1F6;
    border-color: #E4E9F0;
}

/* Harbor button variants - set via the dynamic `variant` property in .ui */
QPushButton[variant="primary"] { background-color: #1D4E89; border: none; color: #FFFFFF; font-weight: 700; }
QPushButton[variant="primary"]:hover { background-color: #163E6C; }
QPushButton[variant="primary"]:pressed { background-color: #11314F; }
QPushButton[variant="accent"] { background-color: #F5A623; border: none; color: #11314F; font-weight: 800; }
QPushButton[variant="accent"]:hover { background-color: #E0900B; }
QPushButton[variant="danger"] { background-color: #D7263D; border: none; color: #FFFFFF; font-weight: 700; }
QPushButton[variant="danger"]:hover { background-color: #B11226; }
QPushButton[variant="ghost"] { background: transparent; border: 1px solid #E4E9F0; color: #1D4E89; font-weight: 700; }
QPushButton[variant="ghost"]:hover { background-color: #EAF1F9; border-color: #BCD3EA; }

QTableWidget, QTreeWidget, QListWidget {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 16px;
    color: #0F2942;
    gridline-color: #EDF1F6;
    selection-background-color: #E7EFF8;
    selection-color: #0E2B47;
    alternate-background-color: #FAFBFD;
    outline: 0;
    padding: 6px;
}

QTableWidget::item, QTreeWidget::item, QListWidget::item { padding: 13px 14px; border: none; border-bottom: 1px solid #EEF2F7; }
QTableWidget::item:hover, QTreeWidget::item:hover, QListWidget::item:hover {
    background-color: #EEF3F9;
}
QTableWidget::item:selected, QTreeWidget::item:selected, QListWidget::item:selected {
    background-color: #E7EFF8;
    color: #0E2B47;
}

QHeaderView::section {
    background-color: #FFFFFF;
    color: #6B7C93;
    border: none;
    border-bottom: 2px solid #E9EDF3;
    padding: 14px 14px;
    font-weight: 700;
}
QTableCornerButton::section { background-color: #FFFFFF; border: none; }

QFrame { background: transparent; }

QScrollBar:vertical { background: transparent; width: 10px; margin: 4px; }
QScrollBar::handle:vertical { background: #C2CEDB; border-radius: 5px; min-height: 36px; }
QScrollBar::handle:vertical:hover { background: #90A0B3; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }
QScrollBar:horizontal { background: transparent; height: 10px; margin: 4px; }
QScrollBar::handle:horizontal { background: #C2CEDB; border-radius: 5px; min-width: 36px; }
QScrollBar::handle:horizontal:hover { background: #90A0B3; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }

QCheckBox, QRadioButton { color: #52647A; spacing: 8px; }

QMenu { background-color: #FFFFFF; border: 1px solid #E4E9F0; border-radius: 12px; padding: 6px; }
QMenu::item { padding: 7px 18px; border-radius: 8px; color: #1B3A57; }
QMenu::item:selected { background-color: #1D4E89; color: #FFFFFF; }

QTabWidget::pane { border: 1px solid #E4E9F0; border-radius: 12px; top: -1px; }
QTabBar::tab {
    background: transparent;
    color: #6B7C93;
    padding: 9px 16px;
    border: none;
    border-bottom: 2px solid transparent;
}
QTabBar::tab:selected { color: #1D4E89; border-bottom: 2px solid #1D4E89; }
QTabBar::tab:hover { color: #33475B; }

QProgressBar {
    background-color: #E7EFF8;
    border: none;
    border-radius: 6px;
    text-align: center;
    color: transparent;
}
QProgressBar::chunk {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2C6BB3, stop:1 #1D4E89);
    border-radius: 6px;
}

QToolTip {
    background-color: #0F2942;
    color: #FFFFFF;
    border: 1px solid #0F2942;
    border-radius: 8px;
    padding: 6px 8px;
}
)";
}

QString Theme::loginStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QMainWindow, QWidget {
    background-color: #F3F5F9;
    color: #1B3A57;
    font-family: "Segoe UI";
}

QFrame, QWidget#loginCard, QWidget#card {
    background-color: transparent;
    border: none;
    border-radius: 0px;
}

QLabel { color: #1B3A57; background: transparent; }
QLabel#titleLabel { font-size: 28px; font-weight: 800; color: #1D4E89; }
QLabel#subtitleLabel { color: #6B7C93; font-size: 13px; }

QLineEdit {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 10px;
    color: #1B3A57;
    padding: 10px 14px;
    min-height: 38px;
    selection-background-color: #1D4E89;
    selection-color: #FFFFFF;
}
QLineEdit:hover { border: 1px solid #C2CEDB; }
QLineEdit:focus { border: 2px solid #F59E0B; padding: 9px 13px; }

QPushButton#loginButton, QPushButton {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2C6BB3, stop:1 #1D4E89);
    border: none;
    border-radius: 8px;
    color: #FFFFFF;
    padding: 11px 16px;
    font-weight: 800;
    min-height: 40px;
}
QPushButton#loginButton:hover, QPushButton:hover { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1D4E89, stop:1 #163E6C); }
QPushButton#loginButton:pressed, QPushButton:pressed { background-color: #163E6C; }

QCheckBox { color: #6B7C93; }

QPushButton#forgotPasswordButton {
    background: transparent;
    border: none;
    color: #1D4E89;
    padding: 0px;
    min-height: 0px;
    text-align: left;
}

QLabel#statusLabel { color: #D7263D; font-weight: 700; }
)";
    }

    return R"(
QMainWindow, QWidget {
    background-color: #0A1A2B;
    color: #F3F5F9;
    font-family: "Segoe UI";
}

QFrame, QWidget#loginCard, QWidget#card {
    background-color: transparent;
    border: none;
    border-radius: 0px;
}

QLabel { color: #F3F5F9; background: transparent; }

QLineEdit {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    color: #F3F5F9;
    padding: 8px 12px;
    min-height: 36px;
}
QLineEdit:focus { border: 1px solid #2C6BB3; }

QPushButton#loginButton, QPushButton {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2C6BB3, stop:1 #1D4E89);
    border: none;
    border-radius: 10px;
    color: #FFFFFF;
    padding: 11px 16px;
    font-weight: 800;
    min-height: 40px;
}
QPushButton#loginButton:hover, QPushButton:hover { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #3D7CC0, stop:1 #2C6BB3); }

QCheckBox { color: #C2CEDB; }

QPushButton#forgotPasswordButton {
    background: transparent;
    border: none;
    color: #7FAAD6;
    padding: 0px;
    min-height: 0px;
    text-align: left;
}

QLabel#statusLabel { color: #FACC15; font-weight: 700; }
)";
}

QString Theme::shellStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QMainWindow#DashboardWindow, QWidget#centralWidget, QStackedWidget#mainStack {
    background-color: #F3F5F9;
}
QWidget#sidebarContainer {
    background-color: #FFFFFF;
    border-right: 1px solid #E4E9F0;
}
QWidget#verticalContainer {
    background-color: #F3F5F9;
}
)";
    }

    return R"(
QMainWindow#DashboardWindow, QWidget#centralWidget, QStackedWidget#mainStack {
    background-color: #0A1A2B;
}
QWidget#sidebarContainer {
    background-color: #0C2236;
    border-right: 1px solid #274463;
}
QWidget#verticalContainer {
    background-color: #0A1A2B;
}
)";
}

QString Theme::sidebarStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#SidebarWidgetClass {
    background-color: #FFFFFF;
    border-right: 1px solid #E4E9F0;
}

QLabel#logoLabel {
    font-size: 18px;
    font-weight: 800;
    color: #0F2942;
}

QLabel#sectionLabel, QLabel#sectionLabel2 {
    color: #6B7C93;
    font-size: 11px;
    font-weight: 800;
    letter-spacing: 1px;
}

QFrame#lineDivider, QFrame#lineDivider2 {
    color: #E4E9F0;
    background-color: #E4E9F0;
    max-height: 1px;
}

QPushButton {
    text-align: left;
    padding: 11px 14px;
    border-radius: 10px;
    border: none;
    background: transparent;
    color: #52647A;
    font-weight: 600;
}
QPushButton:hover { background-color: #EDF1F6; color: #0F2942; }
QPushButton:checked {
    background-color: #FFF4DF;
    color: #B45309;
    font-weight: 800;
}

QFrame#userCard {
    background-color: #F3F5F9;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#avatarLabel {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2C6BB3, stop:1 #163E6C);
    border-radius: 14px;
    color: #FFFFFF;
    font-weight: 800;
}

QLabel#userNameLabel {
    font-size: 12px;
    font-weight: 800;
    color: #0F2942;
}

QLabel#userRoleLabel {
    font-size: 10px;
    color: #6B7C93;
}

QPushButton#logoutButton {
    background: transparent;
    border: none;
    color: #D7263D;
    font-weight: 700;
}
QPushButton#logoutButton:hover { background-color: #FBE3E6; color: #D7263D; }
)";
    }

    return R"(
QWidget#SidebarWidgetClass {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0C2236, stop:1 #0A1A2B);
}

QLabel#logoLabel {
    font-size: 18px;
    font-weight: 800;
    color: #FFFFFF;
}

QLabel#sectionLabel, QLabel#sectionLabel2 {
    color: #9FB2C9;
    font-size: 11px;
    font-weight: 800;
}

QFrame#lineDivider, QFrame#lineDivider2 {
    color: #16314A;
    background-color: #16314A;
    max-height: 1px;
}

QPushButton {
    text-align: left;
    padding: 11px 14px;
    border-radius: 10px;
    border: none;
    background: transparent;
    color: #9FB2C9;
    font-weight: 650;
}
QPushButton:hover { background-color: #16314A; color: #F3F5F9; }
QPushButton:checked {
    background-color: #14385C;
    color: #F5A623;
    font-weight: 800;
}

QFrame#userCard {
    background-color: #0F2438;
    border: 1px solid #274463;
    border-radius: 14px;
}

QLabel#avatarLabel {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2C6BB3, stop:1 #163E6C);
    border-radius: 14px;
    color: #FFFFFF;
    font-weight: 800;
}

QLabel#userNameLabel {
    font-size: 12px;
    font-weight: 800;
    color: #FFFFFF;
}

QLabel#userRoleLabel {
    font-size: 10px;
    color: #9FB2C9;
}

QPushButton#logoutButton {
    background: transparent;
    border: none;
    color: #F2566A;
    font-weight: 700;
}
QPushButton#logoutButton:hover { background-color: #3B1115; color: #F4A6AF; }
)";
}

QString Theme::verticalBarStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#VerticalWidget {
    background-color: #FFFFFF;
    border-bottom: 1px solid #E4E9F0;
}

QLabel#welcomeLabel {
    color: #1B3A57;
    font-size: 22px;
    font-weight: 800;
}
QLabel#subTitleLabel {
    color: #6B7C93;
    font-size: 12px;
}

QFrame#searchContainer {
    background-color: #F3F5F9;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
}

QLineEdit#searchInput {
    background: transparent;
    border: none;
    color: #1B3A57;
    padding: 0px;
    selection-background-color: #1D4E89;
    selection-color: #FFFFFF;
}

QLabel#searchIcon {
    color: #90A0B3;
    font-size: 14px;
}

QLabel#shortcutLabel {
    background-color: #EDF1F6;
    color: #6B7C93;
    border-radius: 6px;
    padding: 4px 8px;
    font-size: 11px;
}

QPushButton#notificationButton {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 17px;
}
QPushButton#notificationButton:hover { background-color: #EAF1F9; border-color: #C2CEDB; }

QLabel#avatarLabel {
    background-color: #1D4E89;
    color: #FFFFFF;
    border-radius: 17px;
    font-weight: 800;
}
)";
    }

    return R"(
QWidget#VerticalWidget { background-color: #0A1A2B; }

QLabel#welcomeLabel {
    color: #FFFFFF;
    font-size: 22px;
    font-weight: 800;
}
QLabel#subTitleLabel {
    color: #90A0B3;
    font-size: 12px;
}

QFrame#searchContainer {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 10px;
}

QLineEdit#searchInput {
    background: transparent;
    border: none;
    color: #FFFFFF;
    padding: 0px;
}

QLabel#searchIcon {
    color: #90A0B3;
    font-size: 14px;
}

QLabel#shortcutLabel {
    background-color: #16314A;
    color: #90A0B3;
    border-radius: 6px;
    padding: 4px 8px;
    font-size: 11px;
}

QPushButton#notificationButton {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 17px;
}
QPushButton#notificationButton:hover { background-color: #16314A; }

QLabel#avatarLabel {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2C6BB3, stop:1 #163E6C);
    color: #FFFFFF;
    border-radius: 17px;
    font-weight: 800;
}
)";
}

QString Theme::dashboardPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#DashboardPageClass { background-color: #F3F5F9; }

QWidget#pageCard, QWidget#summaryCard,
QFrame#itemSummaryCard, QFrame#assetSummaryCard, QFrame#recentItemsCard, QFrame#lowStockCard,
QWidget#itemSummaryCard, QWidget#assetSummaryCard, QWidget#recentItemsCard, QWidget#lowStockCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 18px;
}

/* KPI cards carry a subtle accent gradient matching their metric colour. */
QFrame#totalItemsCard, QWidget#totalItemsCard,
QFrame#totalAssetsCard, QWidget#totalAssetsCard {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFFFFF, stop:1 #EEF3FA);
    border: 1px solid #CFE0F2;
    border-radius: 18px;
}
QFrame#quantityInHandCard, QWidget#quantityInHandCard {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFFFFF, stop:1 #EFFBF3);
    border: 1px solid #CCEFD8;
    border-radius: 18px;
}
QFrame#receivedCard, QWidget#receivedCard {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFFFFF, stop:1 #FFF8EC);
    border: 1px solid #FAE8C2;
    border-radius: 18px;
}

QLabel { color: #1B3A57; }

QLabel#totalItemsTitle, QLabel#totalAssetsTitle, QLabel#quantityTitle, QLabel#receivedTitle,
QLabel#itemLeftLabel, QLabel#itemRightLabel, QLabel#assetActiveLabel, QLabel#assetInactiveLabel {
    color: #6B7C93;
    font-size: 12px;
}

QLabel#totalItemsValue, QLabel#totalAssetsValue, QLabel#quantityInHandValue, QLabel#toBeReceivedValue {
    color: #0F2942;
    font-weight: 800;
    font-size: 26px;
}
QLabel#itemQuantityValue, QLabel#itemReceivedValue, QLabel#assetActiveValue, QLabel#assetInactiveValue {
    color: #0F2942;
    font-weight: 800;
}

QLabel#itemSummaryTitle, QLabel#assetSummaryTitle, QLabel#recentTitle, QLabel#lowStockTitle {
    color: #1B3A57;
    font-weight: 800;
}

QLabel#totalItemsChange, QLabel#totalAssetsChange, QLabel#quantityChange {
    color: #16A34A;
    font-weight: 700;
}
QLabel#receivedChange {
    color: #D7263D;
    font-weight: 700;
}

QLabel#totalItemsIcon, QLabel#totalAssetsIcon {
    background-color: #E7EFF8;
    color: #1D4E89;
    border-radius: 10px;
}
QLabel#quantityIcon {
    background-color: #DCFCE7;
    color: #16A34A;
    border-radius: 10px;
}
QLabel#receivedIcon {
    background-color: #FEF3C7;
    color: #D97706;
    border-radius: 10px;
}

QLabel#itemChartBlue { color: #1D4E89; }
QLabel#itemChartOrange { color: #F97316; }
QLabel#assetChartGreen { color: #16A34A; }
QLabel#assetChartRed { color: #D7263D; }
)";
    }

    return R"(
QWidget#DashboardPageClass { background-color: #0A1A2B; }

QWidget#pageCard, QWidget#summaryCard,
QFrame#itemSummaryCard, QFrame#assetSummaryCard, QFrame#recentItemsCard, QFrame#lowStockCard,
QWidget#itemSummaryCard, QWidget#assetSummaryCard, QWidget#recentItemsCard, QWidget#lowStockCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 18px;
}

/* KPI cards carry a subtle accent gradient matching their metric colour. */
QFrame#totalItemsCard, QWidget#totalItemsCard,
QFrame#totalAssetsCard, QWidget#totalAssetsCard {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0F2A42, stop:1 #16213F);
    border: 1px solid #243B5C;
    border-radius: 18px;
}
QFrame#quantityInHandCard, QWidget#quantityInHandCard {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0F2A42, stop:1 #11271E);
    border: 1px solid #244B39;
    border-radius: 18px;
}
QFrame#receivedCard, QWidget#receivedCard {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0F2A42, stop:1 #2A2310);
    border: 1px solid #4A3A1C;
    border-radius: 18px;
}

QLabel { color: #FFFFFF; }

QLabel#totalItemsTitle, QLabel#totalAssetsTitle, QLabel#quantityTitle, QLabel#receivedTitle,
QLabel#itemLeftLabel, QLabel#itemRightLabel, QLabel#assetActiveLabel, QLabel#assetInactiveLabel {
    color: #C2CEDB;
    font-size: 12px;
}

QLabel#totalItemsValue, QLabel#totalAssetsValue, QLabel#quantityInHandValue, QLabel#toBeReceivedValue {
    color: #FFFFFF;
    font-weight: 800;
    font-size: 26px;
}
QLabel#itemQuantityValue, QLabel#itemReceivedValue, QLabel#assetActiveValue, QLabel#assetInactiveValue {
    color: #FFFFFF;
    font-weight: 800;
}

QLabel#itemSummaryTitle, QLabel#assetSummaryTitle, QLabel#recentTitle, QLabel#lowStockTitle {
    color: #FFFFFF;
    font-weight: 800;
}

QLabel#totalItemsChange, QLabel#totalAssetsChange, QLabel#quantityChange {
    color: #22C55E;
    font-weight: 700;
}
QLabel#receivedChange {
    color: #E5384B;
    font-weight: 700;
}

QLabel#totalItemsIcon, QLabel#totalAssetsIcon {
    background-color: #0E2C49;
    color: #7FAAD6;
    border-radius: 10px;
}
QLabel#quantityIcon {
    background-color: #14532D;
    color: #22C55E;
    border-radius: 10px;
}
QLabel#receivedIcon {
    background-color: #7C3A03;
    color: #F59E0B;
    border-radius: 10px;
}

QLabel#itemChartBlue { color: #2C6BB3; }
QLabel#itemChartOrange { color: #F97316; }
QLabel#assetChartGreen { color: #22C55E; }
QLabel#assetChartRed { color: #E5384B; }
)";
}

QString Theme::settingsPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#SettingsPageClass { background-color: #F3F5F9; }

QWidget#profileCard, QWidget#appearanceCard, QWidget#sessionCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#titleLabel {
    color: #1B3A57;
    font-size: 22px;
    font-weight: 800;
}
QLabel#subtitleLabel {
    color: #6B7C93;
    font-size: 13px;
}
QLabel#profileTitleLabel, QLabel#appearanceTitleLabel, QLabel#sessionTitleLabel {
    color: #1B3A57;
    font-size: 16px;
    font-weight: 800;
}
QLabel#roleValueLabel {
    color: #1D4E89;
    font-weight: 800;
}
QPushButton#saveProfileButton, QPushButton#applyThemeButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}
QPushButton#logoutButton {
    background-color: #D7263D;
    color: #FFFFFF;
    border: none;
}
)";
    }

    return R"(
QWidget#SettingsPageClass { background-color: #0A1A2B; }

QWidget#profileCard, QWidget#appearanceCard, QWidget#sessionCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#titleLabel {
    color: #FFFFFF;
    font-size: 22px;
    font-weight: 800;
}
QLabel#subtitleLabel {
    color: #90A0B3;
    font-size: 13px;
}
QLabel#profileTitleLabel, QLabel#appearanceTitleLabel, QLabel#sessionTitleLabel {
    color: #FFFFFF;
    font-size: 16px;
    font-weight: 800;
}
QLabel#roleValueLabel {
    color: #8FB6E6;
    font-weight: 800;
}
QPushButton#saveProfileButton, QPushButton#applyThemeButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}
QPushButton#logoutButton {
    background-color: #D7263D;
    color: #FFFFFF;
    border: none;
}
)";
}

QString Theme::scanPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(

QWidget#ScanPageClass {
    background-color: #F3F5F9;
}

QWidget#pageCard {
    background-color: transparent;
    border: none;
}

QWidget#scanCard,
QWidget#recentScansCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#pageTitleLabel {
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel,
QLabel#subtitleLabel {
    color: #6B7C93;
    font-size: 12px;
}

QLabel#titleLabel {
    color: #1B3A57;
    font-size: 15px;
    font-weight: 800;
}

QLabel#quantityLabel,
QLabel#recentTitleLabel {
    color: #1B3A57;
    font-weight: 800;
}

QLineEdit#barcodeInput {
    background-color: #FFFFFF;
    border: 1px solid #1D4E89;
    border-top-left-radius: 8px;
    border-bottom-left-radius: 8px;
    border-top-right-radius: 0px;
    border-bottom-right-radius: 0px;
    color: #1B3A57;
    padding: 0px 12px;
    min-height: 44px;
    max-height: 44px;
}

QLabel#barcodeIconLabel {
    background-color: #FFFFFF;
    border: 1px solid #1D4E89;
    border-left: none;
    border-top-right-radius: 8px;
    border-bottom-right-radius: 8px;
    color: #1D4E89;
    font-size: 18px;
}

QPushButton#minusButton,
QPushButton#plusButton {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    color: #1B3A57;
    font-size: 16px;
    font-weight: 800;
}

QSpinBox#quantityInput {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-left: none;
    border-right: none;
    color: #1B3A57;
    padding: 0px;
    min-height: 44px;
    max-height: 44px;
}

QPushButton#submitButton {
    background-color: #16A34A;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    font-weight: 800;
}

QPushButton#submitButton:hover {
    background-color: #22C55E;
}

QLabel#statusLabel {
    color: #16A34A;
    font-weight: 800;
}

QLabel#recentCodeLabel,
QLabel#recentCodeLabel_2,
QLabel#recentCodeLabel_3 {
    color: #33475B;
    font-weight: 800;
}

QLabel#recentNameLabel,
QLabel#recentNameLabel_2,
QLabel#recentNameLabel_3,
QLabel#recentDateLabel,
QLabel#recentDateLabel_2,
QLabel#recentDateLabel_3 {
    color: #6B7C93;
}

QLabel#recentQtyLabel,
QLabel#recentQtyLabel_2,
QLabel#recentQtyLabel_3 {
    color: #16A34A;
    font-weight: 800;
}

)";
    }

    return R"(

QWidget#ScanPageClass {
    background-color: #0A1A2B;
}

QWidget#pageCard {
    background-color: transparent;
    border: none;
}

QWidget#scanCard,
QWidget#recentScansCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#pageTitleLabel {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel,
QLabel#subtitleLabel {
    color: #90A0B3;
    font-size: 12px;
}

QLabel#titleLabel {
    color: #FFFFFF;
    font-size: 15px;
    font-weight: 800;
}

QLabel#quantityLabel,
QLabel#recentTitleLabel {
    color: #FFFFFF;
    font-weight: 800;
}

QLineEdit#barcodeInput {
    background-color: #0A1A2B;
    border: 1px solid #2C6BB3;
    border-top-left-radius: 8px;
    border-bottom-left-radius: 8px;
    border-top-right-radius: 0px;
    border-bottom-right-radius: 0px;
    color: #C2CEDB;
    padding: 0px 12px;
    min-height: 44px;
    max-height: 44px;
}

QLabel#barcodeIconLabel {
    background-color: #0A1A2B;
    border: 1px solid #2C6BB3;
    border-left: none;
    border-top-right-radius: 8px;
    border-bottom-right-radius: 8px;
    color: #8FB6E6;
    font-size: 18px;
}

QPushButton#minusButton,
QPushButton#plusButton {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    color: #C2CEDB;
    font-size: 16px;
    font-weight: 800;
}

QSpinBox#quantityInput {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    border-left: none;
    border-right: none;
    color: #FFFFFF;
    padding: 0px;
    min-height: 44px;
    max-height: 44px;
}

QPushButton#submitButton {
    background-color: #16A34A;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    font-weight: 800;
}

QPushButton#submitButton:hover {
    background-color: #22C55E;
}

QLabel#statusLabel {
    color: #22C55E;
    font-weight: 800;
}

QLabel#recentCodeLabel,
QLabel#recentCodeLabel_2,
QLabel#recentCodeLabel_3 {
    color: #C2CEDB;
    font-weight: 800;
}

QLabel#recentNameLabel,
QLabel#recentNameLabel_2,
QLabel#recentNameLabel_3,
QLabel#recentDateLabel,
QLabel#recentDateLabel_2,
QLabel#recentDateLabel_3 {
    color: #90A0B3;
}

QLabel#recentQtyLabel,
QLabel#recentQtyLabel_2,
QLabel#recentQtyLabel_3 {
    color: #22C55E;
    font-weight: 800;
}

)";
}


QString Theme::itemsPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return dataPageStyle(theme) + R"(

QWidget#ItemsPageClass {
    background-color: #F3F5F9;
}

QWidget#pageContainer {
    background-color: transparent;
}

QWidget#toolbarContainer {
    background-color: transparent;
    border: none;
}

QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #6B7C93;
    font-size: 12px;
}

QLineEdit#itemSearchInput {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
    color: #1B3A57;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addItemButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QPushButton#filterItemButton,
QPushButton#itemSearchButton {
    background-color: #FFFFFF;
    color: #1B3A57;
    border: 1px solid #E4E9F0;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #52647A;
}

QPushButton#activePageButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#ItemsPageClass {
    background-color: #0A1A2B;
}

QWidget#pageContainer {
    background-color: transparent;
}

QWidget#toolbarContainer {
    background-color: transparent;
    border: none;
}

QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #90A0B3;
    font-size: 12px;
}

QLineEdit#itemSearchInput {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    color: #C2CEDB;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addItemButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QPushButton#filterItemButton,
QPushButton#itemSearchButton {
    background-color: #0F2438;
    color: #FFFFFF;
    border: 1px solid #1E3A52;
}

QWidget#paginationContainer {
    background-color: #0F2438;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #C2CEDB;
}

QPushButton#activePageButton {
    background-color: #163E6C;
    color: #FFFFFF;
    border: none;
}

)";
}

QString Theme::assetsPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return dataPageStyle(theme) + R"(

QWidget#AssetsPageClass {
    background-color: #F3F5F9;
}

QWidget#pageContainer {
    background-color: transparent;
}

QWidget#toolbarContainer {
    background-color: transparent;
    border: none;
}

QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #6B7C93;
    font-size: 12px;
}

QLineEdit#assetSearchInput {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
    color: #1B3A57;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addAssetButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QPushButton#filterAssetButton,
QPushButton#assetSearchButton {
    background-color: #FFFFFF;
    color: #1B3A57;
    border: 1px solid #E4E9F0;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #52647A;
}

QPushButton#activePageButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#AssetsPageClass {
    background-color: #0A1A2B;
}

QWidget#pageContainer {
    background-color: transparent;
}

QWidget#toolbarContainer {
    background-color: transparent;
    border: none;
}

QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #90A0B3;
    font-size: 12px;
}

QLineEdit#assetSearchInput {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    color: #C2CEDB;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addAssetButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QPushButton#filterAssetButton,
QPushButton#assetSearchButton {
    background-color: #0F2438;
    color: #FFFFFF;
    border: 1px solid #1E3A52;
}

QWidget#paginationContainer {
    background-color: #0F2438;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #C2CEDB;
}

QPushButton#activePageButton {
    background-color: #163E6C;
    color: #FFFFFF;
    border: none;
}

)";
}

QString Theme::usersPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return dataPageStyle(theme) + R"(

QWidget#UsersPageClass {
    background-color: #F3F5F9;
}

QWidget#pageContainer {
    background-color: transparent;
}

QWidget#toolbarContainer {
    background-color: transparent;
    border: none;
}

QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #6B7C93;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
    color: #1B3A57;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QComboBox#roleFilter {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
    color: #1B3A57;
    padding: 0px 10px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addUserButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #52647A;
}

QPushButton#activePageButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#UsersPageClass {
    background-color: #0A1A2B;
}

QWidget#pageContainer {
    background-color: transparent;
}

QWidget#toolbarContainer {
    background-color: transparent;
    border: none;
}

QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #90A0B3;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    color: #C2CEDB;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QComboBox#roleFilter {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    color: #C2CEDB;
    padding: 0px 10px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addUserButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #0F2438;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #C2CEDB;
}

QPushButton#activePageButton {
    background-color: #163E6C;
    color: #FFFFFF;
    border: none;
}

)";
}

QString Theme::truckStockDashboardPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(

* {
    font-family: "Segoe UI";
    font-size: 13px;
}

QWidget#TruckStockDashboardPageClass {
    background-color: #F3F5F9;
    color: #1B3A57;
}

QLabel {
    background-color: transparent;
    border: none;
    color: #1B3A57;
}

QLabel#pageTitle {
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#subtitleLabel {
    color: #6B7C93;
    font-size: 12px;
}

QWidget#itemSummaryCard,
QWidget#assetSummaryCard,
QWidget#recentItemsCard,
QWidget#lowStockCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 18px;
}

/* Four truck KPI cards, each with its own accent gradient. */
QWidget#metricCard1 {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFFFFF, stop:1 #EEF3FA);
    border: 1px solid #CFE0F2;
    border-radius: 18px;
}
QWidget#metricCard2 {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFFFFF, stop:1 #EFFBF3);
    border: 1px solid #CCEFD8;
    border-radius: 18px;
}
QWidget#metricCard3 {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFFFFF, stop:1 #FFF8EC);
    border: 1px solid #FAE8C2;
    border-radius: 18px;
}
QWidget#metricCard4 {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFFFFF, stop:1 #EAF1F9);
    border: 1px solid #CFE0F2;
    border-radius: 18px;
}

QWidget#miniSummaryBox1,
QWidget#miniSummaryBox2,
QWidget#miniSummaryBox3,
QWidget#miniSummaryBox4 {
    background-color: #F3F5F9;
    border: 1px solid #E4E9F0;
    border-radius: 10px;
}

QWidget#metricCard1 QLabel,
QWidget#metricCard2 QLabel,
QWidget#metricCard3 QLabel,
QWidget#metricCard4 QLabel,
QWidget#itemSummaryCard QLabel,
QWidget#assetSummaryCard QLabel,
QWidget#recentItemsCard QLabel,
QWidget#lowStockCard QLabel,
QWidget#miniSummaryBox1 QLabel,
QWidget#miniSummaryBox2 QLabel,
QWidget#miniSummaryBox3 QLabel,
QWidget#miniSummaryBox4 QLabel {
    background-color: transparent;
    border: none;
}

QLabel#metricTitle1,
QLabel#metricTitle2,
QLabel#metricTitle3,
QLabel#metricTitle4,
QLabel#summarySmallLabel1,
QLabel#summarySmallLabel2,
QLabel#summarySmallLabel3,
QLabel#summarySmallLabel4 {
    color: #6B7C93;
    font-size: 12px;
    font-weight: 700;
}

QLabel#metricValue1,
QLabel#metricValue2,
QLabel#metricValue3,
QLabel#metricValue4,
QLabel#summaryValue1,
QLabel#summaryValue2,
QLabel#summaryValue3,
QLabel#summaryValue4 {
    color: #1B3A57;
    font-size: 26px;
    font-weight: 900;
}

QLabel#positiveLabel1,
QLabel#positiveLabel2,
QLabel#positiveLabel3,
QLabel#summaryPositive1,
QLabel#summaryPositive2 {
    color: #16A34A;
    font-size: 12px;
    font-weight: 700;
}

QLabel#negativeLabel1,
QLabel#summaryNegative1,
QLabel#summaryNegative2 {
    color: #D7263D;
    font-size: 12px;
    font-weight: 700;
}

QLabel#cardTitle1,
QLabel#cardTitle2,
QLabel#cardTitle3,
QLabel#cardTitle4 {
    color: #1B3A57;
    font-size: 16px;
    font-weight: 800;
}

QWidget#blueIconBox,
QWidget#blueIconBox2 {
    background-color: #E7EFF8;
    border: none;
    border-radius: 10px;
}

QWidget#greenIconBox {
    background-color: #DCFCE7;
    border: none;
    border-radius: 10px;
}

QWidget#orangeIconBox {
    background-color: #FFEDD5;
    border: none;
    border-radius: 10px;
}

QLabel#blueIconLabel,
QLabel#blueIconLabel2 {
    color: #1D4E89;
    font-size: 22px;
}

QLabel#greenIconLabel {
    color: #16A34A;
    font-size: 22px;
}

QLabel#orangeIconLabel {
    color: #EA580C;
    font-size: 22px;
}

QLabel#blueSparkline {
    color: #1D4E89;
    font-size: 18px;
}

QLabel#orangeSparkline {
    color: #EA580C;
    font-size: 18px;
}

QLabel#greenSparkline {
    color: #16A34A;
    font-size: 18px;
}

QLabel#redSparkline {
    color: #D7263D;
    font-size: 18px;
}

QPushButton#viewAllItemsButton,
QPushButton#viewAllLowStockButton {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    color: #1D4E89;
    border-radius: 8px;
    padding: 5px 10px;
    font-weight: 700;
}

QPushButton#viewAllItemsButton:hover,
QPushButton#viewAllLowStockButton:hover {
    background-color: #E7EFF8;
}

QTableWidget {
    background-color: transparent;
    border: none;
    color: #1B3A57;
    gridline-color: transparent;
    outline: none;
}

QTableWidget::item {
    background-color: transparent;
    border-bottom: 1px solid #E4E9F0;
    padding-left: 8px;
}

QHeaderView::section {
    background-color: transparent;
    color: #6B7C93;
    border: none;
    border-bottom: 1px solid #E4E9F0;
    padding-left: 8px;
    font-size: 12px;
    font-weight: 700;
}

)";
    }

    return R"(

* {
    font-family: "Segoe UI";
    font-size: 13px;
}

QWidget#TruckStockDashboardPageClass {
    background-color: #0A1A2B;
    color: #FFFFFF;
}

QLabel {
    background-color: transparent;
    border: none;
    color: #FFFFFF;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#subtitleLabel {
    color: #90A0B3;
    font-size: 12px;
}

QWidget#itemSummaryCard,
QWidget#assetSummaryCard,
QWidget#recentItemsCard,
QWidget#lowStockCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 18px;
}

/* Four truck KPI cards, each with its own accent gradient. */
QWidget#metricCard1 {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0F2A42, stop:1 #16213F);
    border: 1px solid #243B5C;
    border-radius: 18px;
}
QWidget#metricCard2 {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0F2A42, stop:1 #11271E);
    border: 1px solid #244B39;
    border-radius: 18px;
}
QWidget#metricCard3 {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0F2A42, stop:1 #2A2310);
    border: 1px solid #4A3A1C;
    border-radius: 18px;
}
QWidget#metricCard4 {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0F2A42, stop:1 #0F2A42);
    border: 1px solid #1E466A;
    border-radius: 18px;
}

QWidget#miniSummaryBox1,
QWidget#miniSummaryBox2,
QWidget#miniSummaryBox3,
QWidget#miniSummaryBox4 {
    background-color: #0F2438;
    border: 1px solid #1B3A52;
    border-radius: 10px;
}

QWidget#metricCard1 QLabel,
QWidget#metricCard2 QLabel,
QWidget#metricCard3 QLabel,
QWidget#metricCard4 QLabel,
QWidget#itemSummaryCard QLabel,
QWidget#assetSummaryCard QLabel,
QWidget#recentItemsCard QLabel,
QWidget#lowStockCard QLabel,
QWidget#miniSummaryBox1 QLabel,
QWidget#miniSummaryBox2 QLabel,
QWidget#miniSummaryBox3 QLabel,
QWidget#miniSummaryBox4 QLabel {
    background-color: transparent;
    border: none;
}

QLabel#metricTitle1,
QLabel#metricTitle2,
QLabel#metricTitle3,
QLabel#metricTitle4,
QLabel#summarySmallLabel1,
QLabel#summarySmallLabel2,
QLabel#summarySmallLabel3,
QLabel#summarySmallLabel4 {
    color: #90A0B3;
    font-size: 12px;
    font-weight: 700;
}

QLabel#metricValue1,
QLabel#metricValue2,
QLabel#metricValue3,
QLabel#metricValue4,
QLabel#summaryValue1,
QLabel#summaryValue2,
QLabel#summaryValue3,
QLabel#summaryValue4 {
    color: #FFFFFF;
    font-size: 26px;
    font-weight: 900;
}

QLabel#positiveLabel1,
QLabel#positiveLabel2,
QLabel#positiveLabel3,
QLabel#summaryPositive1,
QLabel#summaryPositive2 {
    color: #22C55E;
    font-size: 12px;
    font-weight: 700;
}

QLabel#negativeLabel1,
QLabel#summaryNegative1,
QLabel#summaryNegative2 {
    color: #F2566A;
    font-size: 12px;
    font-weight: 700;
}

QLabel#cardTitle1,
QLabel#cardTitle2,
QLabel#cardTitle3,
QLabel#cardTitle4 {
    color: #FFFFFF;
    font-size: 16px;
    font-weight: 800;
}

QWidget#blueIconBox,
QWidget#blueIconBox2 {
    background-color: #0E2C49;
    border: none;
    border-radius: 10px;
}

QWidget#greenIconBox {
    background-color: #14532D;
    border: none;
    border-radius: 10px;
}

QWidget#orangeIconBox {
    background-color: #7C2D12;
    border: none;
    border-radius: 10px;
}

QLabel#blueIconLabel,
QLabel#blueIconLabel2 {
    color: #8FB6E6;
    font-size: 22px;
}

QLabel#greenIconLabel {
    color: #86EFAC;
    font-size: 22px;
}

QLabel#orangeIconLabel {
    color: #FDBA74;
    font-size: 22px;
}

QLabel#blueSparkline {
    color: #2C6BB3;
    font-size: 18px;
}

QLabel#orangeSparkline {
    color: #F97316;
    font-size: 18px;
}

QLabel#greenSparkline {
    color: #22C55E;
    font-size: 18px;
}

QLabel#redSparkline {
    color: #E5384B;
    font-size: 18px;
}

QPushButton#viewAllItemsButton,
QPushButton#viewAllLowStockButton {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    color: #7FAAD6;
    border-radius: 8px;
    padding: 5px 10px;
    font-weight: 700;
}

QPushButton#viewAllItemsButton:hover,
QPushButton#viewAllLowStockButton:hover {
    background-color: #16314A;
}

QTableWidget {
    background-color: transparent;
    border: none;
    color: #FFFFFF;
    gridline-color: transparent;
    outline: none;
}

QTableWidget::item {
    background-color: transparent;
    border-bottom: 1px solid #16314A;
    padding-left: 8px;
}

QHeaderView::section {
    background-color: transparent;
    color: #90A0B3;
    border: none;
    border-bottom: 1px solid #1B3A52;
    padding-left: 8px;
    font-size: 12px;
    font-weight: 700;
}

)";
}

QString Theme::trucksPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return dataPageStyle(theme) + R"(

QWidget#TrucksPageClass {
    background-color: #F3F5F9;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #6B7C93;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
    color: #1B3A57;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addTruckButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #52647A;
}

QPushButton#activePageButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#TrucksPageClass {
    background-color: #0A1A2B;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #90A0B3;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    color: #C2CEDB;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addTruckButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #0F2438;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #C2CEDB;
}

QPushButton#activePageButton {
    background-color: #163E6C;
    color: #FFFFFF;
    border: none;
}

)";
}

QString Theme::templatesPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return dataPageStyle(theme) + R"(

QWidget#StockTemplatesPageClass {
    background-color: #F3F5F9;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #6B7C93;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
    color: #1B3A57;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#newTemplateButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #52647A;
}

QPushButton#activePageButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#StockTemplatesPageClass {
    background-color: #0A1A2B;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #90A0B3;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    color: #C2CEDB;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#newTemplateButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #0F2438;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #C2CEDB;
}

QPushButton#activePageButton {
    background-color: #163E6C;
    color: #FFFFFF;
    border: none;
}

)";
}

QString Theme::assignmentsPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return dataPageStyle(theme) + R"(

QWidget#AssignmentsPageClass {
    background-color: #F3F5F9;
}

QWidget#pageContainer,
QWidget#toolbarContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #6B7C93;
    font-size: 12px;
}

QPushButton#assignTemplateButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #52647A;
}

QPushButton#activePageButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#AssignmentsPageClass {
    background-color: #0A1A2B;
}

QWidget#pageContainer,
QWidget#toolbarContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #90A0B3;
    font-size: 12px;
}

QPushButton#assignTemplateButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #0F2438;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #C2CEDB;
}

QPushButton#activePageButton {
    background-color: #163E6C;
    color: #FFFFFF;
    border: none;
}

)";
}

QString Theme::myTruckStockPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return dataPageStyle(theme) + R"(

QWidget#MyTruckStockPageClass {
    background-color: #F3F5F9;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #6B7C93;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
    color: #1B3A57;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#useButton {
    background-color: #16A34A;
    border: none;
    color: #FFFFFF;
    border-radius: 6px;
    padding: 4px 10px;
    font-weight: 800;
}

QPushButton#useButton:hover {
    background-color: #22C55E;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#MyTruckStockPageClass {
    background-color: #0A1A2B;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #90A0B3;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    color: #C2CEDB;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#useButton {
    background-color: #16A34A;
    border: none;
    color: #FFFFFF;
    border-radius: 6px;
    padding: 4px 10px;
    font-weight: 800;
}

QPushButton#useButton:hover {
    background-color: #22C55E;
}

)";
}

QString Theme::lowStockAlertsPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return dataPageStyle(theme) + R"(

QWidget#LowStockAlertsPageClass {
    background-color: #F3F5F9;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#metricCard1,
QWidget#metricCard2,
QWidget#metricCard3 {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
    min-height: 105px;
}

QWidget#metricCard1 QLabel,
QWidget#metricCard2 QLabel,
QWidget#metricCard3 QLabel {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    background-color: transparent;
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    background-color: transparent;
    color: #6B7C93;
    font-size: 12px;
}

QLabel#metricTitle,
QLabel#metricTitle1,
QLabel#metricTitle2 {
    color: #6B7C93;
    font-size: 12px;
    font-weight: 700;
}

QLabel#metricValue {
    color: #D7263D;
    font-size: 28px;
    font-weight: 900;
}

QLabel#metricValue1 {
    color: #D97706;
    font-size: 28px;
    font-weight: 900;
}

QLabel#metricValue2 {
    color: #16A34A;
    font-size: 28px;
    font-weight: 900;
}

QLabel#dangerText {
    color: #D7263D;
    font-size: 12px;
}

QLabel#warningText {
    color: #D97706;
    font-size: 12px;
}

QLabel#okText {
    color: #16A34A;
    font-size: 12px;
}

QLineEdit#searchInput,
QComboBox#statusFilter {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
    color: #1B3A57;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#exportButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #52647A;
}

QPushButton#activePageButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#LowStockAlertsPageClass {
    background-color: #0A1A2B;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#metricCard1,
QWidget#metricCard2,
QWidget#metricCard3 {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
    min-height: 105px;
}

QWidget#metricCard1 QLabel,
QWidget#metricCard2 QLabel,
QWidget#metricCard3 QLabel {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#pageTitle {
    background-color: transparent;
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    background-color: transparent;
    color: #90A0B3;
    font-size: 12px;
}

QLabel#metricTitle,
QLabel#metricTitle1,
QLabel#metricTitle2 {
    color: #90A0B3;
    font-size: 12px;
    font-weight: 700;
}

QLabel#metricValue {
    color: #E5384B;
    font-size: 28px;
    font-weight: 900;
}

QLabel#metricValue1 {
    color: #F59E0B;
    font-size: 28px;
    font-weight: 900;
}

QLabel#metricValue2 {
    color: #22C55E;
    font-size: 28px;
    font-weight: 900;
}

QLabel#dangerText {
    color: #F2566A;
    font-size: 12px;
}

QLabel#warningText {
    color: #FBBF24;
    font-size: 12px;
}

QLabel#okText {
    color: #22C55E;
    font-size: 12px;
}

QLineEdit#searchInput,
QComboBox#statusFilter {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    color: #C2CEDB;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#exportButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #0F2438;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #C2CEDB;
}

QPushButton#activePageButton {
    background-color: #163E6C;
    color: #FFFFFF;
    border: none;
}

)";
}

QString Theme::receiptsPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return dataPageStyle(theme) + R"(

QWidget#ReceiptsPageClass {
    background-color: #F3F5F9;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #6B7C93;
    font-size: 12px;
}

QLineEdit#searchInput,
QComboBox#statusFilter {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
    color: #1B3A57;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#uploadButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QPushButton#approveButton {
    background-color: #DCFCE7;
    border: 1px solid #86EFAC;
    color: #16A34A;
    border-radius: 6px;
    padding: 4px 8px;
    font-weight: 800;
}

QPushButton#approveButton:hover {
    background-color: #16A34A;
    color: #FFFFFF;
}

QPushButton#approveButton:disabled {
    background-color: #E4E9F0;
    border: 1px solid #C7D0DC;
    color: #90A0B3;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#ReceiptsPageClass {
    background-color: #0A1A2B;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #90A0B3;
    font-size: 12px;
}

QLineEdit#searchInput,
QComboBox#statusFilter {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    color: #C2CEDB;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#uploadButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QPushButton#approveButton {
    background-color: #052E16;
    border: 1px solid #166534;
    color: #22C55E;
    border-radius: 6px;
    padding: 4px 8px;
    font-weight: 800;
}

QPushButton#approveButton:hover {
    background-color: #16A34A;
    color: #FFFFFF;
}

QPushButton#approveButton:disabled {
    background-color: #0F2942;
    border: 1px solid #33475B;
    color: #6B7C93;
}

)";
}

QString Theme::notificationDialogStyle(AppTheme theme)
{
    QString background =
        theme == AppTheme::Light ? "#F3F5F9" : "#0F2942";

    QString card =
        theme == AppTheme::Light ? "#FFFFFF" : "#0F2942";

    QString text =
        theme == AppTheme::Light ? "#0F2942" : "#E4E9F0";

    QString muted =
        theme == AppTheme::Light ? "#6B7C93" : "#90A0B3";

    QString border =
        theme == AppTheme::Light ? "#E4E9F0" : "#16314A";

    QString button =
        theme == AppTheme::Light ? "#1D4E89" : "#2C6BB3";

    QString buttonText =
        "#FFFFFF";

    return QString(R"(
        QDialog {
            background-color: %1;
            color: %3;
        }

        QLabel#titleLabel {
            color: %3;
            font-size: 20px;
            font-weight: 700;
        }

        QLabel#summaryLabel {
            color: %4;
            font-size: 13px;
        }

        QTextEdit#notificationText {
            background-color: %2;
            color: %3;
            border: 1px solid %5;
            border-radius: 10px;
            padding: 10px;
            font-size: 13px;
        }

        QPushButton {
            background-color: %6;
            color: %7;
            border: none;
            border-radius: 8px;
            padding: 8px 14px;
            font-weight: 600;
        }

        QPushButton:hover {
            background-color: #163E6C;
        }

        QPushButton#closeButton {
            background-color: transparent;
            color: %3;
            border: 1px solid %5;
        }

        QPushButton#closeButton:hover {
            background-color: %2;
        }
    )")
        .arg(background)
        .arg(card)
        .arg(text)
        .arg(muted)
        .arg(border)
        .arg(button)
        .arg(buttonText);
}

QString Theme::dataPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget {
    background-color: #F3F5F9;
    color: #1B3A57;
}

QWidget#pageCard, QWidget#card, QWidget#tableCard, QWidget#toolbarCard, QWidget#formCard, QWidget#summaryCard,
QFrame#pageCard, QFrame#card, QFrame#tableCard, QFrame#toolbarCard, QFrame#formCard, QFrame#summaryCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel { color: #1B3A57; }

QLabel#titleLabel, QLabel#pageTitleLabel, QLabel#sectionTitleLabel {
    color: #1B3A57;
    font-weight: 800;
}

QLabel#subtitleLabel, QLabel#breadcrumbLabel, QLabel#descriptionLabel {
    color: #6B7C93;
}

QPushButton#addItemButton, QPushButton#addAssetButton, QPushButton#addButton, QPushButton#newButton,
QPushButton#saveButton, QPushButton#saveTruckButton, QPushButton#saveProfileButton,
QPushButton#applyThemeButton, QPushButton#uploadReceiptButton, QPushButton#assignTemplateButton,
QPushButton#submitButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QPushButton#deleteButton, QPushButton#logoutButton {
    background-color: #D7263D;
    color: #FFFFFF;
    border: none;
}

QPushButton#cancelButton {
    background-color: #FFFFFF;
    color: #1B3A57;
    border: 1px solid #E4E9F0;
}

QTableWidget {
    background-color: transparent;
    border: none;
    color: #1B3A57;
    gridline-color: transparent;
    outline: none;
}

QTableWidget::item {
    background-color: transparent;
    border-bottom: 1px solid #F0F3F8;
    padding: 11px 12px;
    color: #33475B;
}

QTableWidget::item:hover {
    background-color: #F5F8FC;
}

QTableWidget::item:selected {
    background-color: #E7EFF8;
    color: #0F2942;
}

QHeaderView::section {
    background-color: #EDF1F6;
    color: #6B7C93;
    border: none;
    border-bottom: 1px solid #E4E9F0;
    padding-left: 12px;
    height: 42px;
    font-size: 12px;
    font-weight: 600;
}

QScrollBar:vertical {
    background: transparent;
    width: 6px;
    margin: 4px 0 4px 0;
}

QScrollBar::handle:vertical {
    background: rgba(100, 116, 139, 0.35);
    border-radius: 3px;
    min-height: 30px;
}

QScrollBar::handle:vertical:hover {
    background: rgba(100, 116, 139, 0.65);
}

QScrollBar::handle:vertical:pressed {
    background: rgba(100, 116, 139, 0.9);
}

QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0;
}

QScrollBar::add-page:vertical,
QScrollBar::sub-page:vertical {
    background: transparent;
}

QWidget#actionContainer {
    background-color: transparent;
    border: none;
}

QPushButton#viewButton,
QPushButton#editButton,
QPushButton#resetButton {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    color: #33475B;
    border-radius: 6px;
    padding: 4px 7px;
}

QPushButton#viewButton:hover,
QPushButton#editButton:hover,
QPushButton#resetButton:hover {
    background-color: #EDF1F6;
    border-color: #C2CEDB;
}

QPushButton#deleteButton {
    background-color: #FBE3E6;
    border: 1px solid #F4A6AF;
    color: #D7263D;
    border-radius: 6px;
    padding: 4px 7px;
}

QPushButton#deleteButton:hover {
    background-color: #F7CDD3;
}

)";
    }

    return R"(
QWidget {
    background-color: #0A1A2B;
    color: #FFFFFF;
}

QWidget#pageCard, QWidget#card, QWidget#tableCard, QWidget#toolbarCard, QWidget#formCard, QWidget#summaryCard,
QFrame#pageCard, QFrame#card, QFrame#tableCard, QFrame#toolbarCard, QFrame#formCard, QFrame#summaryCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel { color: #FFFFFF; }

QLabel#titleLabel, QLabel#pageTitleLabel, QLabel#sectionTitleLabel {
    color: #FFFFFF;
    font-weight: 800;
}

QLabel#subtitleLabel, QLabel#breadcrumbLabel, QLabel#descriptionLabel {
    color: #90A0B3;
}

QPushButton#addItemButton, QPushButton#addAssetButton, QPushButton#addButton, QPushButton#newButton,
QPushButton#saveButton, QPushButton#saveTruckButton, QPushButton#saveProfileButton,
QPushButton#applyThemeButton, QPushButton#uploadReceiptButton, QPushButton#assignTemplateButton,
QPushButton#submitButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

QPushButton#deleteButton, QPushButton#logoutButton {
    background-color: #D7263D;
    color: #FFFFFF;
    border: none;
}

QPushButton#cancelButton {
    background-color: #0F2438;
    color: #FFFFFF;
    border: 1px solid #1E3A52;
}

QTableWidget {
    background-color: transparent;
    border: none;
    color: #FFFFFF;
    gridline-color: transparent;
    outline: none;
}

QTableWidget::item {
    background-color: transparent;
    border-bottom: 1px solid #16314A;
    padding: 11px 12px;
    color: #C2CEDB;
}

QTableWidget::item:hover {
    background-color: #142B42;
}

QTableWidget::item:selected {
    background-color: #1B3F5E;
    color: #FFFFFF;
}

QHeaderView::section {
    background-color: #091826;
    color: #90A0B3;
    border: none;
    border-bottom: 1px solid #1B3A52;
    padding-left: 12px;
    height: 42px;
    font-size: 12px;
    font-weight: 600;
}

QScrollBar:vertical {
    background: transparent;
    width: 6px;
    margin: 4px 0 4px 0;
}

QScrollBar::handle:vertical {
    background: rgba(140, 140, 140, 0.35);
    border-radius: 3px;
    min-height: 30px;
}

QScrollBar::handle:vertical:hover {
    background: rgba(180, 180, 180, 0.65);
}

QScrollBar::handle:vertical:pressed {
    background: rgba(200, 200, 200, 0.9);
}

QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0;
}

QScrollBar::add-page:vertical,
QScrollBar::sub-page:vertical {
    background: transparent;
}

QWidget#actionContainer {
    background-color: transparent;
    border: none;
}

QPushButton#viewButton,
QPushButton#editButton,
QPushButton#resetButton {
    background-color: #16314A;
    border: 1px solid #1E3A52;
    color: #C2CEDB;
    border-radius: 6px;
    padding: 4px 7px;
}

QPushButton#viewButton:hover,
QPushButton#editButton:hover,
QPushButton#resetButton:hover {
    background-color: #1B3F5E;
    border-color: #274463;
}

QPushButton#deleteButton {
    background-color: #3B1115;
    border: 1px solid #7A1722;
    color: #F2566A;
    border-radius: 6px;
    padding: 4px 7px;
}

QPushButton#deleteButton:hover {
    background-color: #511820;
}

)";
}

QString Theme::dialogStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(

QDialog {
    background-color: transparent;
    color: #1B3A57;
    font-family: "Segoe UI";
    font-size: 13px;
}

QWidget#dialogCard {
    background-color: #FFFFFF;
    border: none;
    border-radius: 14px;
}

QWidget#headerContainer {
    background-color: #1D4E89;
    border-top-left-radius: 12px;
    border-top-right-radius: 12px;
    min-height: 48px;
    max-height: 48px;
}

QWidget#formContainer,
QWidget#footerContainer {
    background-color: transparent;
    border: none;
}

QLabel {
    background-color: transparent;
    border: none;
    color: #1B3A57;
    font-weight: 600;
}

QLabel#titleLabel {
    color: #FFFFFF;
    font-size: 18px;
    font-weight: 700;
}

QLabel#statusLabel, QLabel#errorLabel {
    color: #D7263D;
    font-weight: 700;
}

QLineEdit,
QComboBox,
QSpinBox,
QTextEdit,
QPlainTextEdit {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 6px;
    color: #1B3A57;
    padding: 4px 10px;
    min-height: 28px;
    max-height: 32px;
}

QLineEdit:focus,
QComboBox:focus,
QSpinBox:focus,
QTextEdit:focus,
QPlainTextEdit:focus {
    border: 1px solid #1D4E89;
}

QPushButton {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 6px;
    color: #1B3A57;
    padding: 6px 16px;
    font-weight: 600;
    min-width: 110px;
    min-height: 36px;
    max-height: 36px;
}

QPushButton:hover {
    background-color: #EDF1F6;
}

QPushButton#saveButton {
    background-color: #1D4E89;
    border: none;
    color: #FFFFFF;
}

QPushButton#saveButton:hover {
    background-color: #163E6C;
}

QPushButton#cancelButton {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    color: #1B3A57;
}

QPushButton#closeButton {
    background-color: transparent;
    border: none;
    color: #FFFFFF;
    min-width: 28px;
    max-width: 28px;
    min-height: 28px;
    max-height: 28px;
    padding: 0px;
    font-size: 16px;
    font-weight: 700;
}

QPushButton#closeButton:hover {
    background-color: rgba(255, 255, 255, 0.18);
}

)";
    }

    return R"(

QDialog {
    background-color: transparent;
    color: #FFFFFF;
    font-family: "Segoe UI";
    font-size: 13px;
}

QWidget#dialogCard {
    background-color: #0F2438;
    border: none;
    border-radius: 14px;
}

QWidget#headerContainer {
    background-color: #163E6C;
    border-top-left-radius: 12px;
    border-top-right-radius: 12px;
    min-height: 48px;
    max-height: 48px;
}

QWidget#formContainer,
QWidget#footerContainer {
    background-color: transparent;
    border: none;
}

QLabel {
    background-color: transparent;
    border: none;
    color: #FFFFFF;
    font-weight: 600;
}

QLabel#titleLabel {
    color: #FFFFFF;
    font-size: 18px;
    font-weight: 700;
}

QLabel#statusLabel, QLabel#errorLabel {
    color: #F2566A;
    font-weight: 700;
}

QLineEdit,
QComboBox,
QSpinBox,
QTextEdit,
QPlainTextEdit {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    border-radius: 6px;
    color: #FFFFFF;
    padding: 4px 10px;
    min-height: 28px;
    max-height: 32px;
}

QLineEdit:focus,
QComboBox:focus,
QSpinBox:focus,
QTextEdit:focus,
QPlainTextEdit:focus {
    border: 1px solid #2C6BB3;
}

QPushButton {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 6px;
    color: #FFFFFF;
    padding: 6px 16px;
    font-weight: 600;
    min-width: 110px;
    min-height: 36px;
    max-height: 36px;
}

QPushButton:hover {
    background-color: #16314A;
}

QPushButton#saveButton {
    background-color: #1D4E89;
    border: none;
    color: #FFFFFF;
}

QPushButton#saveButton:hover {
    background-color: #163E6C;
}

QPushButton#cancelButton {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    color: #FFFFFF;
}

QPushButton#closeButton {
    background-color: transparent;
    border: none;
    color: #FFFFFF;
    min-width: 28px;
    max-width: 28px;
    min-height: 28px;
    max-height: 28px;
    padding: 0px;
    font-size: 16px;
    font-weight: 700;
}

QPushButton#closeButton:hover {
    background-color: rgba(255, 255, 255, 0.14);
}

QWidget#leftCard,
QWidget#rightCard {
    background-color: transparent;
    border: none;
}

QWidget#quantityCard {
    background-color: transparent;
    border: none;
}

QWidget#uploadArea,
QWidget#summaryCard {
    background-color: transparent;
    border: none;
}

)";
}

QString Theme::truckPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return dataPageStyle(theme) + R"(

QWidget#TruckStockDashboardPageClass,
QWidget#TrucksPageClass,
QWidget#StockTemplatesPageClass,
QWidget#AssignmentsPageClass,
QWidget#MyTruckStockPageClass,
QWidget#LowStockAlertsPageClass,
QWidget#ReceiptsPageClass {
    background-color: #F3F5F9;
    color: #1B3A57;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard,
QWidget#metricCard1,
QWidget#metricCard2,
QWidget#metricCard3,
QWidget#metricCard4,
QWidget#itemSummaryCard,
QWidget#assetSummaryCard,
QWidget#recentItemsCard,
QWidget#lowStockCard,
QWidget#miniSummaryBox1,
QWidget#miniSummaryBox2,
QWidget#miniSummaryBox3,
QWidget#miniSummaryBox4 {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel {
    background-color: transparent;
    border: none;
    color: #1B3A57;
}

QLabel#pageTitle,
QLabel#titleLabel,
QLabel#cardTitle1,
QLabel#cardTitle2,
QLabel#cardTitle3,
QLabel#cardTitle4 {
    background-color: transparent;
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel,
QLabel#subtitleLabel {
    background-color: transparent;
    color: #6B7C93;
    font-size: 12px;
}

QLineEdit#searchInput,
QComboBox#statusFilter,
QComboBox#roleFilter {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
    color: #1B3A57;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QTableWidget,
QTableView {
    background-color: #FFFFFF;
    border: none;
    color: #1B3A57;
    gridline-color: transparent;
    selection-background-color: #E7EFF8;
    selection-color: #1B3A57;
}

QTableWidget::item,
QTableView::item {
    background-color: #FFFFFF;
    color: #1B3A57;
    border-bottom: 1px solid #E4E9F0;
    padding-left: 8px;
}

QTableWidget::item:selected,
QTableView::item:selected {
    background-color: #E7EFF8;
    color: #1B3A57;
}

QHeaderView::section {
    background-color: #FFFFFF;
    color: #6B7C93;
    border: none;
    border-bottom: 1px solid #E4E9F0;
    padding-left: 8px;
    font-size: 12px;
    font-weight: 700;
}

QPushButton#addTruckButton,
QPushButton#newTemplateButton,
QPushButton#assignTemplateButton,
QPushButton#exportButton,
QPushButton#uploadButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    font-weight: 800;
}

QPushButton#viewButton {
    background-color: #E7EFF8;
    border: 1px solid #8FB6E6;
    color: #1D4E89;
    border-radius: 6px;
    padding: 4px 8px;
    font-weight: 800;
}

QPushButton#editButton {
    background-color: #FEF3C7;
    border: 1px solid #FCD34D;
    color: #D97706;
    border-radius: 6px;
    padding: 4px 8px;
    font-weight: 800;
}

QPushButton#deleteButton {
    background-color: #FBE3E6;
    border: 1px solid #F4A6AF;
    color: #D7263D;
    border-radius: 6px;
    padding: 4px 8px;
    font-weight: 800;
}

QPushButton#approveButton,
QPushButton#useButton {
    background-color: #DCFCE7;
    border: 1px solid #86EFAC;
    color: #16A34A;
    border-radius: 6px;
    padding: 4px 8px;
    font-weight: 800;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #52647A;
}

QPushButton#activePageButton {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#TruckStockDashboardPageClass,
QWidget#TrucksPageClass,
QWidget#StockTemplatesPageClass,
QWidget#AssignmentsPageClass,
QWidget#MyTruckStockPageClass,
QWidget#LowStockAlertsPageClass,
QWidget#ReceiptsPageClass {
    background-color: #0A1A2B;
    color: #FFFFFF;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard,
QWidget#metricCard1,
QWidget#metricCard2,
QWidget#metricCard3,
QWidget#metricCard4,
QWidget#itemSummaryCard,
QWidget#assetSummaryCard,
QWidget#recentItemsCard,
QWidget#lowStockCard,
QWidget#miniSummaryBox1,
QWidget#miniSummaryBox2,
QWidget#miniSummaryBox3,
QWidget#miniSummaryBox4 {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel {
    background-color: transparent;
    border: none;
    color: #FFFFFF;
}

QLabel#pageTitle,
QLabel#titleLabel,
QLabel#cardTitle1,
QLabel#cardTitle2,
QLabel#cardTitle3,
QLabel#cardTitle4 {
    background-color: transparent;
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel,
QLabel#subtitleLabel {
    background-color: transparent;
    color: #90A0B3;
    font-size: 12px;
}

QLineEdit#searchInput,
QComboBox#statusFilter,
QComboBox#roleFilter {
    background-color: #0A1A2B;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    color: #C2CEDB;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QTableWidget,
QTableView {
    background-color: #0F2438;
    border: none;
    color: #FFFFFF;
    gridline-color: transparent;
    selection-background-color: #163E6C;
    selection-color: #FFFFFF;
}

QTableWidget::item,
QTableView::item {
    background-color: #0F2438;
    color: #FFFFFF;
    border-bottom: 1px solid #16314A;
    padding-left: 8px;
}

QHeaderView::section {
    background-color: #0F2438;
    color: #90A0B3;
    border: none;
    border-bottom: 1px solid #1B3A52;
    padding-left: 8px;
    font-size: 12px;
    font-weight: 700;
}

QWidget#paginationContainer {
    background-color: #0F2438;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #C2CEDB;
}

QPushButton#activePageButton {
    background-color: #163E6C;
    color: #FFFFFF;
    border: none;
}

)";
}


QString Theme::reportsPageStyle(AppTheme theme)
{
    return dataPageStyle(theme) + (theme == AppTheme::Light ? R"(

QWidget#ReportsPageClass {
    background-color: #F3F5F9;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#buttonContainer,
QWidget#exportContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard,
QFrame#tableCard,
QWidget#summaryCard,
QFrame#summaryCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#pageTitle,
QLabel#titleLabel {
    color: #1B3A57;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel,
QLabel#subtitleLabel {
    color: #6B7C93;
    font-size: 12px;
}

QWidget#reportTypeCard {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 14px;
}

QLabel#reportTypeLabel, QLabel#exportHintLabel {
    color: #6B7C93;
    font-weight: 700;
}

QPushButton#inventorySummaryBtn, QPushButton#assetsSummaryBtn, QPushButton#truckSummaryBtn,
QPushButton#templatesSummaryBtn, QPushButton#assignmentsSummaryBtn, QPushButton#receiptsSummaryBtn,
QPushButton#lowStockSummaryBtn, QPushButton#technicianStockSummaryBtn, QPushButton#refreshBtn {
    background-color: #FFFFFF;
    color: #33475B;
    border: 1px solid #E4E9F0;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 700;
    text-align: left;
}

QPushButton#inventorySummaryBtn:hover, QPushButton#assetsSummaryBtn:hover, QPushButton#truckSummaryBtn:hover,
QPushButton#templatesSummaryBtn:hover, QPushButton#assignmentsSummaryBtn:hover, QPushButton#receiptsSummaryBtn:hover,
QPushButton#lowStockSummaryBtn:hover, QPushButton#technicianStockSummaryBtn:hover, QPushButton#refreshBtn:hover {
    background-color: #EAF1F9;
    border-color: #BCD3EA;
}

QPushButton#inventorySummaryBtn:checked, QPushButton#assetsSummaryBtn:checked, QPushButton#truckSummaryBtn:checked,
QPushButton#templatesSummaryBtn:checked, QPushButton#assignmentsSummaryBtn:checked, QPushButton#receiptsSummaryBtn:checked,
QPushButton#lowStockSummaryBtn:checked, QPushButton#technicianStockSummaryBtn:checked {
    background-color: #1D4E89;
    color: #FFFFFF;
    border-color: #1D4E89;
}

QPushButton#exportPdfBtn {
    background-color: #D7263D;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 800;
}

QPushButton#exportPdfBtn:hover {
    background-color: #E5384B;
}

QPushButton#exportExcelBtn {
    background-color: #16A34A;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 800;
}

QPushButton#exportExcelBtn:hover {
    background-color: #22C55E;
}

QPushButton#exportCsvBtn {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 800;
}

QPushButton#exportCsvBtn:hover {
    background-color: #2C6BB3;
}

QTableWidget#reportTable,
QTableWidget {
    background-color: #FFFFFF;
    border: 1px solid #E4E9F0;
    border-radius: 10px;
    color: #1B3A57;
    gridline-color: #E4E9F0;
    selection-background-color: #E7EFF8;
    selection-color: #1B3A57;
    alternate-background-color: #F3F5F9;
}

QTableWidget::item {
    padding: 8px;
    border-bottom: 1px solid #E4E9F0;
}

QHeaderView::section {
    background-color: #E4E9F0;
    color: #33475B;
    border: none;
    border-bottom: 1px solid #E4E9F0;
    padding: 8px;
    font-weight: 800;
}

)" : R"(

QWidget#ReportsPageClass {
    background-color: #0A1A2B;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#buttonContainer,
QWidget#exportContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard,
QFrame#tableCard,
QWidget#summaryCard,
QFrame#summaryCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#pageTitle,
QLabel#titleLabel {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel,
QLabel#subtitleLabel {
    color: #90A0B3;
    font-size: 12px;
}

QWidget#reportTypeCard {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 14px;
}

QLabel#reportTypeLabel, QLabel#exportHintLabel {
    color: #90A0B3;
    font-weight: 700;
}

QPushButton#inventorySummaryBtn, QPushButton#assetsSummaryBtn, QPushButton#truckSummaryBtn,
QPushButton#templatesSummaryBtn, QPushButton#assignmentsSummaryBtn, QPushButton#receiptsSummaryBtn,
QPushButton#lowStockSummaryBtn, QPushButton#technicianStockSummaryBtn, QPushButton#refreshBtn {
    background-color: #0F2438;
    color: #C2CEDB;
    border: 1px solid #1E3A52;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 700;
    text-align: left;
}

QPushButton#inventorySummaryBtn:hover, QPushButton#assetsSummaryBtn:hover, QPushButton#truckSummaryBtn:hover,
QPushButton#templatesSummaryBtn:hover, QPushButton#assignmentsSummaryBtn:hover, QPushButton#receiptsSummaryBtn:hover,
QPushButton#lowStockSummaryBtn:hover, QPushButton#technicianStockSummaryBtn:hover, QPushButton#refreshBtn:hover {
    background-color: #16314A;
    border-color: #274463;
}

QPushButton#inventorySummaryBtn:checked, QPushButton#assetsSummaryBtn:checked, QPushButton#truckSummaryBtn:checked,
QPushButton#templatesSummaryBtn:checked, QPushButton#assignmentsSummaryBtn:checked, QPushButton#receiptsSummaryBtn:checked,
QPushButton#lowStockSummaryBtn:checked, QPushButton#technicianStockSummaryBtn:checked {
    background-color: #2C6BB3;
    color: #FFFFFF;
    border-color: #2C6BB3;
}

QPushButton#exportPdfBtn {
    background-color: #D7263D;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 800;
}

QPushButton#exportPdfBtn:hover {
    background-color: #E5384B;
}

QPushButton#exportExcelBtn {
    background-color: #16A34A;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 800;
}

QPushButton#exportExcelBtn:hover {
    background-color: #22C55E;
}

QPushButton#exportCsvBtn {
    background-color: #1D4E89;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 800;
}

QPushButton#exportCsvBtn:hover {
    background-color: #2C6BB3;
}

QTableWidget#reportTable,
QTableWidget {
    background-color: #0F2438;
    border: 1px solid #1E3A52;
    border-radius: 10px;
    color: #F3F5F9;
    gridline-color: #16314A;
    selection-background-color: #163E6C;
    selection-color: #FFFFFF;
    alternate-background-color: #102A41;
}

QTableWidget::item {
    padding: 8px;
    border-bottom: 1px solid #16314A;
}

QHeaderView::section {
    background-color: #102A41;
    color: #C2CEDB;
    border: none;
    border-bottom: 1px solid #1E3A52;
    padding: 8px;
    font-weight: 800;
}

)");
}
