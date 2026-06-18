#include "Theme.h"

#include <QApplication>

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

    widget->setStyleSheet(loginStyle(AppTheme::Light));
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
    background-color: #0A1020;
    color: #F8FAFC;
}

QLabel {
    color: #F8FAFC;
    background: transparent;
}

QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QTextEdit, QPlainTextEdit, QDateEdit {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 10px;
    color: #F8FAFC;
    padding: 9px 12px;
    selection-background-color: #4F46E5;
    selection-color: #FFFFFF;
}

QLineEdit:hover, QComboBox:hover, QSpinBox:hover, QDoubleSpinBox:hover,
QTextEdit:hover, QPlainTextEdit:hover, QDateEdit:hover {
    border-color: #3C4D70;
}

QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus,
QTextEdit:focus, QPlainTextEdit:focus, QDateEdit:focus {
    border: 1px solid #6366F1;
    background-color: #141E36;
}

QComboBox::drop-down { border: none; width: 22px; }
QComboBox::down-arrow {
    image: none;
    width: 0px; height: 0px;
    border-left: 4px solid transparent;
    border-right: 4px solid transparent;
    border-top: 5px solid #94A3B8;
    margin-right: 10px;
}
QComboBox QAbstractItemView {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    color: #F8FAFC;
    padding: 4px;
    outline: 0;
    selection-background-color: #4F46E5;
    selection-color: #FFFFFF;
}

QPushButton {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1C2740, stop:1 #141D31);
    border: 1px solid #2E3D5B;
    border-radius: 10px;
    color: #F8FAFC;
    padding: 9px 16px;
    font-weight: 700;
}

QPushButton:hover {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #25324F, stop:1 #1A2440);
    border-color: #6366F1;
}

QPushButton:pressed {
    background-color: #161F34;
}

QPushButton:checked {
    background-color: #4F46E5;
    border-color: #4F46E5;
    color: #FFFFFF;
}

QPushButton:disabled {
    color: #64748B;
    background-color: #0F1626;
    border-color: #243352;
}

QTableWidget, QTreeWidget, QListWidget {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
    color: #F8FAFC;
    gridline-color: #1E2942;
    selection-background-color: #4F46E5;
    selection-color: #FFFFFF;
    alternate-background-color: #152138;
    outline: 0;
}

QTableWidget::item, QTreeWidget::item, QListWidget::item {
    padding: 7px 8px;
    border: none;
}
QTableWidget::item:hover, QTreeWidget::item:hover, QListWidget::item:hover {
    background-color: #1B2843;
}
QTableWidget::item:selected, QTreeWidget::item:selected, QListWidget::item:selected {
    background-color: #4F46E5;
    color: #FFFFFF;
}

QHeaderView::section {
    background-color: #16213B;
    color: #94A3B8;
    border: none;
    border-bottom: 1px solid #2E3D5B;
    padding: 10px 8px;
    font-weight: 700;
}
QTableCornerButton::section { background-color: #16213B; border: none; }

QFrame { background: transparent; }

QScrollBar:vertical {
    background: transparent;
    width: 10px;
    margin: 4px;
}
QScrollBar::handle:vertical {
    background: #2E3D5B;
    border-radius: 5px;
    min-height: 36px;
}
QScrollBar::handle:vertical:hover { background: #4F46E5; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }
QScrollBar:horizontal {
    background: transparent;
    height: 10px;
    margin: 4px;
}
QScrollBar::handle:horizontal {
    background: #2E3D5B;
    border-radius: 5px;
    min-width: 36px;
}
QScrollBar::handle:horizontal:hover { background: #4F46E5; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }

QCheckBox, QRadioButton { color: #CBD5E1; spacing: 8px; }

QMenu {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 10px;
    padding: 6px;
}
QMenu::item { padding: 7px 18px; border-radius: 6px; color: #F8FAFC; }
QMenu::item:selected { background-color: #4F46E5; color: #FFFFFF; }

QTabWidget::pane { border: 1px solid #2E3D5B; border-radius: 10px; top: -1px; }
QTabBar::tab {
    background: transparent;
    color: #94A3B8;
    padding: 9px 16px;
    border: none;
    border-bottom: 2px solid transparent;
}
QTabBar::tab:selected { color: #F8FAFC; border-bottom: 2px solid #4F46E5; }
QTabBar::tab:hover { color: #CBD5E1; }

QProgressBar {
    background-color: #121C30;
    border: none;
    border-radius: 6px;
    text-align: center;
    color: transparent;
}
QProgressBar::chunk {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366F1, stop:1 #4F46E5);
    border-radius: 6px;
}

QToolTip {
    background-color: #16213B;
    color: #F8FAFC;
    border: 1px solid #4F46E5;
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
    background-color: #F8FAFC;
    color: #1E293B;
}

QLabel {
    color: #1E293B;
    background: transparent;
}

QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QTextEdit, QPlainTextEdit, QDateEdit {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #1E293B;
    padding: 9px 12px;
    selection-background-color: #4F46E5;
    selection-color: #FFFFFF;
}

QLineEdit:hover, QComboBox:hover, QSpinBox:hover, QDoubleSpinBox:hover,
QTextEdit:hover, QPlainTextEdit:hover, QDateEdit:hover {
    border-color: #CBD5E1;
}

QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus,
QTextEdit:focus, QPlainTextEdit:focus, QDateEdit:focus {
    border: 2px solid #4F46E5;
    padding: 8px 11px;
}

QComboBox::drop-down { border: none; width: 22px; }
QComboBox::down-arrow {
    image: none;
    width: 0px; height: 0px;
    border-left: 4px solid transparent;
    border-right: 4px solid transparent;
    border-top: 5px solid #94A3B8;
    margin-right: 10px;
}
QComboBox QAbstractItemView {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #1E293B;
    padding: 4px;
    outline: 0;
    selection-background-color: #4F46E5;
    selection-color: #FFFFFF;
}

QPushButton {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #334155;
    padding: 9px 16px;
    font-weight: 600;
}

QPushButton:hover {
    background-color: #F8FAFC;
    border-color: #CBD5E1;
    color: #0F172A;
}

QPushButton:pressed {
    background-color: #F1F5F9;
}

QPushButton:checked {
    background-color: #4F46E5;
    border-color: #4F46E5;
    color: #FFFFFF;
}

QPushButton:disabled {
    color: #94A3B8;
    background-color: #F1F5F9;
    border-color: #E2E8F0;
}

QTableWidget, QTreeWidget, QListWidget {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
    color: #1E293B;
    gridline-color: #EFF2F6;
    selection-background-color: #EEF2FF;
    selection-color: #312E81;
    alternate-background-color: #FBFCFE;
    outline: 0;
}

QTableWidget::item, QTreeWidget::item, QListWidget::item { padding: 11px 12px; border: none; }
QTableWidget::item:hover, QTreeWidget::item:hover, QListWidget::item:hover {
    background-color: #F5F7FB;
}
QTableWidget::item:selected, QTreeWidget::item:selected, QListWidget::item:selected {
    background-color: #EEF2FF;
    color: #312E81;
}

QHeaderView::section {
    background-color: #F8FAFC;
    color: #475569;
    border: none;
    border-bottom: 1px solid #E2E8F0;
    padding: 12px 12px;
    font-weight: 700;
}
QTableCornerButton::section { background-color: #F8FAFC; border: none; }

QFrame { background: transparent; }

QScrollBar:vertical { background: transparent; width: 10px; margin: 4px; }
QScrollBar::handle:vertical { background: #CBD5E1; border-radius: 5px; min-height: 36px; }
QScrollBar::handle:vertical:hover { background: #94A3B8; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }
QScrollBar:horizontal { background: transparent; height: 10px; margin: 4px; }
QScrollBar::handle:horizontal { background: #CBD5E1; border-radius: 5px; min-width: 36px; }
QScrollBar::handle:horizontal:hover { background: #94A3B8; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }

QCheckBox, QRadioButton { color: #475569; spacing: 8px; }

QMenu { background-color: #FFFFFF; border: 1px solid #E2E8F0; border-radius: 12px; padding: 6px; }
QMenu::item { padding: 7px 18px; border-radius: 8px; color: #1E293B; }
QMenu::item:selected { background-color: #4F46E5; color: #FFFFFF; }

QTabWidget::pane { border: 1px solid #E2E8F0; border-radius: 12px; top: -1px; }
QTabBar::tab {
    background: transparent;
    color: #64748B;
    padding: 9px 16px;
    border: none;
    border-bottom: 2px solid transparent;
}
QTabBar::tab:selected { color: #4F46E5; border-bottom: 2px solid #4F46E5; }
QTabBar::tab:hover { color: #334155; }

QProgressBar {
    background-color: #EEF2FF;
    border: none;
    border-radius: 6px;
    text-align: center;
    color: transparent;
}
QProgressBar::chunk {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366F1, stop:1 #4F46E5);
    border-radius: 6px;
}

QToolTip {
    background-color: #0F172A;
    color: #FFFFFF;
    border: 1px solid #0F172A;
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
    background-color: #F8FAFC;
    color: #1E293B;
    font-family: "Segoe UI";
}

QFrame, QWidget#loginCard, QWidget#card {
    background-color: transparent;
    border: none;
    border-radius: 0px;
}

QLabel { color: #1E293B; background: transparent; }
QLabel#titleLabel { font-size: 28px; font-weight: 800; color: #4F46E5; }
QLabel#subtitleLabel { color: #64748B; font-size: 13px; }

QLineEdit {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 10px;
    color: #1E293B;
    padding: 10px 14px;
    min-height: 38px;
    selection-background-color: #4F46E5;
    selection-color: #FFFFFF;
}
QLineEdit:hover { border: 1px solid #CBD5E1; }
QLineEdit:focus { border: 2px solid #4F46E5; padding: 9px 13px; }

QPushButton#loginButton, QPushButton {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #6366F1, stop:1 #4F46E5);
    border: none;
    border-radius: 8px;
    color: #FFFFFF;
    padding: 11px 16px;
    font-weight: 800;
    min-height: 40px;
}
QPushButton#loginButton:hover, QPushButton:hover { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4F46E5, stop:1 #4338CA); }
QPushButton#loginButton:pressed, QPushButton:pressed { background-color: #4338CA; }

QCheckBox { color: #64748B; }

QPushButton#forgotPasswordButton {
    background: transparent;
    border: none;
    color: #4F46E5;
    padding: 0px;
    min-height: 0px;
    text-align: left;
}

QLabel#statusLabel { color: #DC2626; font-weight: 700; }
)";
    }

    return R"(
QMainWindow, QWidget {
    background-color: #0A1020;
    color: #F8FAFC;
    font-family: "Segoe UI";
}

QFrame, QWidget#loginCard, QWidget#card {
    background-color: transparent;
    border: none;
    border-radius: 0px;
}

QLabel { color: #F8FAFC; background: transparent; }

QLineEdit {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    color: #F8FAFC;
    padding: 8px 12px;
    min-height: 36px;
}
QLineEdit:focus { border: 1px solid #6366F1; }

QPushButton#loginButton, QPushButton {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #6366F1, stop:1 #4F46E5);
    border: none;
    border-radius: 10px;
    color: #FFFFFF;
    padding: 11px 16px;
    font-weight: 800;
    min-height: 40px;
}
QPushButton#loginButton:hover, QPushButton:hover { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #7C7DF4, stop:1 #5B52E8); }

QCheckBox { color: #CBD5E1; }

QPushButton#forgotPasswordButton {
    background: transparent;
    border: none;
    color: #818CF8;
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
    background-color: #F8FAFC;
}
QWidget#sidebarContainer {
    background-color: #FFFFFF;
    border-right: 1px solid #E2E8F0;
}
QWidget#verticalContainer {
    background-color: #F8FAFC;
}
)";
    }

    return R"(
QMainWindow#DashboardWindow, QWidget#centralWidget, QStackedWidget#mainStack {
    background-color: #0A1020;
}
QWidget#sidebarContainer {
    background-color: #0E1729;
    border-right: 1px solid #243352;
}
QWidget#verticalContainer {
    background-color: #0A1020;
}
)";
}

QString Theme::sidebarStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#SidebarWidgetClass {
    background-color: #FFFFFF;
    border-right: 1px solid #E2E8F0;
}

QLabel#logoLabel {
    font-size: 18px;
    font-weight: 800;
    color: #1E293B;
}

QLabel#sectionLabel, QLabel#sectionLabel2 {
    color: #64748B;
    font-size: 11px;
    font-weight: 800;
    letter-spacing: 1px;
}

QFrame#lineDivider, QFrame#lineDivider2 {
    color: #E2E8F0;
    background-color: #E2E8F0;
    max-height: 1px;
}

QPushButton {
    text-align: left;
    padding: 11px 14px;
    border-radius: 10px;
    border: none;
    background: transparent;
    color: #475569;
    font-weight: 600;
}
QPushButton:hover { background-color: #F1F5F9; color: #0F172A; }
QPushButton:checked {
    background-color: #EEF2FF;
    color: #4338CA;
    font-weight: 700;
}

QFrame#userCard {
    background-color: #F8FAFC;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#avatarLabel {
    background-color: #4F46E5;
    border-radius: 14px;
    color: #FFFFFF;
    font-weight: 800;
}

QLabel#userNameLabel {
    font-size: 12px;
    font-weight: 800;
    color: #1E293B;
}

QLabel#userRoleLabel {
    font-size: 10px;
    color: #64748B;
}

QPushButton#logoutButton {
    background: transparent;
    border: none;
    color: #DC2626;
    font-weight: 700;
}
QPushButton#logoutButton:hover { background-color: #FEE2E2; color: #DC2626; }
)";
    }

    return R"(
QWidget#SidebarWidgetClass {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0E1729, stop:1 #0A1020);
}

QLabel#logoLabel {
    font-size: 18px;
    font-weight: 800;
    color: #FFFFFF;
}

QLabel#sectionLabel, QLabel#sectionLabel2 {
    color: #8493AE;
    font-size: 11px;
    font-weight: 800;
}

QFrame#lineDivider, QFrame#lineDivider2 {
    color: #1B2742;
    background-color: #1B2742;
    max-height: 1px;
}

QPushButton {
    text-align: left;
    padding: 11px 14px;
    border-radius: 10px;
    border: none;
    background: transparent;
    color: #C7D2E4;
    font-weight: 650;
}
QPushButton:hover { background-color: #18223C; color: #F8FAFC; }
QPushButton:checked {
    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #5B52E8, stop:1 #4F46E5);
    color: #FFFFFF;
    font-weight: 800;
}

QFrame#userCard {
    background-color: #121C30;
    border: 1px solid #2A3953;
    border-radius: 14px;
}

QLabel#avatarLabel {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #6366F1, stop:1 #4338CA);
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
    color: #8493AE;
}

QPushButton#logoutButton {
    background: transparent;
    border: none;
    color: #F87171;
    font-weight: 700;
}
QPushButton#logoutButton:hover { background-color: #3B1115; color: #FCA5A5; }
)";
}

QString Theme::verticalBarStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#VerticalWidget {
    background-color: #FFFFFF;
    border-bottom: 1px solid #E2E8F0;
}

QLabel#welcomeLabel {
    color: #1E293B;
    font-size: 22px;
    font-weight: 800;
}
QLabel#subTitleLabel {
    color: #64748B;
    font-size: 12px;
}

QFrame#searchContainer {
    background-color: #F8FAFC;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
}

QLineEdit#searchInput {
    background: transparent;
    border: none;
    color: #1E293B;
    padding: 0px;
    selection-background-color: #4F46E5;
    selection-color: #FFFFFF;
}

QLabel#searchIcon {
    color: #94A3B8;
    font-size: 14px;
}

QLabel#shortcutLabel {
    background-color: #F1F5F9;
    color: #64748B;
    border-radius: 6px;
    padding: 4px 8px;
    font-size: 11px;
}

QPushButton#notificationButton {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 17px;
}
QPushButton#notificationButton:hover { background-color: #EFF6FF; border-color: #CBD5E1; }

QLabel#avatarLabel {
    background-color: #4F46E5;
    color: #FFFFFF;
    border-radius: 17px;
    font-weight: 800;
}
)";
    }

    return R"(
QWidget#VerticalWidget { background-color: #0A1020; }

QLabel#welcomeLabel {
    color: #FFFFFF;
    font-size: 22px;
    font-weight: 800;
}
QLabel#subTitleLabel {
    color: #94A3B8;
    font-size: 12px;
}

QFrame#searchContainer {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 10px;
}

QLineEdit#searchInput {
    background: transparent;
    border: none;
    color: #FFFFFF;
    padding: 0px;
}

QLabel#searchIcon {
    color: #94A3B8;
    font-size: 14px;
}

QLabel#shortcutLabel {
    background-color: #1B2843;
    color: #94A3B8;
    border-radius: 6px;
    padding: 4px 8px;
    font-size: 11px;
}

QPushButton#notificationButton {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 17px;
}
QPushButton#notificationButton:hover { background-color: #1B2843; }

QLabel#avatarLabel {
    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #6366F1, stop:1 #4338CA);
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
QWidget#DashboardPageClass { background-color: #F8FAFC; }

QWidget#pageCard, QWidget#summaryCard,
QFrame#totalItemsCard, QFrame#totalAssetsCard, QFrame#quantityInHandCard, QFrame#receivedCard,
QFrame#itemSummaryCard, QFrame#assetSummaryCard, QFrame#recentItemsCard, QFrame#lowStockCard,
QWidget#totalItemsCard, QWidget#totalAssetsCard, QWidget#quantityInHandCard, QWidget#receivedCard,
QWidget#itemSummaryCard, QWidget#assetSummaryCard, QWidget#recentItemsCard, QWidget#lowStockCard {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel { color: #1E293B; }

QLabel#totalItemsTitle, QLabel#totalAssetsTitle, QLabel#quantityTitle, QLabel#receivedTitle,
QLabel#itemLeftLabel, QLabel#itemRightLabel, QLabel#assetActiveLabel, QLabel#assetInactiveLabel {
    color: #64748B;
    font-size: 12px;
}

QLabel#totalItemsValue, QLabel#totalAssetsValue, QLabel#quantityInHandValue, QLabel#toBeReceivedValue,
QLabel#itemQuantityValue, QLabel#itemReceivedValue, QLabel#assetActiveValue, QLabel#assetInactiveValue {
    color: #1E293B;
    font-weight: 800;
}

QLabel#itemSummaryTitle, QLabel#assetSummaryTitle, QLabel#recentTitle, QLabel#lowStockTitle {
    color: #1E293B;
    font-weight: 800;
}

QLabel#totalItemsChange, QLabel#totalAssetsChange, QLabel#quantityChange {
    color: #16A34A;
    font-weight: 700;
}
QLabel#receivedChange {
    color: #DC2626;
    font-weight: 700;
}

QLabel#totalItemsIcon, QLabel#totalAssetsIcon {
    background-color: #EEF2FF;
    color: #4F46E5;
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

QLabel#itemChartBlue { color: #4F46E5; }
QLabel#itemChartOrange { color: #F97316; }
QLabel#assetChartGreen { color: #16A34A; }
QLabel#assetChartRed { color: #DC2626; }
)";
    }

    return R"(
QWidget#DashboardPageClass { background-color: #0A1020; }

QWidget#pageCard, QWidget#summaryCard,
QFrame#totalItemsCard, QFrame#totalAssetsCard, QFrame#quantityInHandCard, QFrame#receivedCard,
QFrame#itemSummaryCard, QFrame#assetSummaryCard, QFrame#recentItemsCard, QFrame#lowStockCard,
QWidget#totalItemsCard, QWidget#totalAssetsCard, QWidget#quantityInHandCard, QWidget#receivedCard,
QWidget#itemSummaryCard, QWidget#assetSummaryCard, QWidget#recentItemsCard, QWidget#lowStockCard {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QLabel { color: #FFFFFF; }

QLabel#totalItemsTitle, QLabel#totalAssetsTitle, QLabel#quantityTitle, QLabel#receivedTitle,
QLabel#itemLeftLabel, QLabel#itemRightLabel, QLabel#assetActiveLabel, QLabel#assetInactiveLabel {
    color: #CBD5E1;
    font-size: 12px;
}

QLabel#totalItemsValue, QLabel#totalAssetsValue, QLabel#quantityInHandValue, QLabel#toBeReceivedValue,
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
    color: #EF4444;
    font-weight: 700;
}

QLabel#totalItemsIcon, QLabel#totalAssetsIcon {
    background-color: #211C54;
    color: #818CF8;
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

QLabel#itemChartBlue { color: #6366F1; }
QLabel#itemChartOrange { color: #F97316; }
QLabel#assetChartGreen { color: #22C55E; }
QLabel#assetChartRed { color: #EF4444; }
)";
}

QString Theme::settingsPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#SettingsPageClass { background-color: #F8FAFC; }

QWidget#profileCard, QWidget#appearanceCard, QWidget#sessionCard {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#titleLabel {
    color: #1E293B;
    font-size: 22px;
    font-weight: 800;
}
QLabel#subtitleLabel {
    color: #64748B;
    font-size: 13px;
}
QLabel#profileTitleLabel, QLabel#appearanceTitleLabel, QLabel#sessionTitleLabel {
    color: #1E293B;
    font-size: 16px;
    font-weight: 800;
}
QLabel#roleValueLabel {
    color: #4F46E5;
    font-weight: 800;
}
QPushButton#saveProfileButton, QPushButton#applyThemeButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}
QPushButton#logoutButton {
    background-color: #DC2626;
    color: #FFFFFF;
    border: none;
}
)";
    }

    return R"(
QWidget#SettingsPageClass { background-color: #0A1020; }

QWidget#profileCard, QWidget#appearanceCard, QWidget#sessionCard {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QLabel#titleLabel {
    color: #FFFFFF;
    font-size: 22px;
    font-weight: 800;
}
QLabel#subtitleLabel {
    color: #94A3B8;
    font-size: 13px;
}
QLabel#profileTitleLabel, QLabel#appearanceTitleLabel, QLabel#sessionTitleLabel {
    color: #FFFFFF;
    font-size: 16px;
    font-weight: 800;
}
QLabel#roleValueLabel {
    color: #93C5FD;
    font-weight: 800;
}
QPushButton#saveProfileButton, QPushButton#applyThemeButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}
QPushButton#logoutButton {
    background-color: #DC2626;
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
    background-color: #F8FAFC;
}

QWidget#pageCard {
    background-color: transparent;
    border: none;
}

QWidget#scanCard,
QWidget#recentScansCard {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#pageTitleLabel {
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel,
QLabel#subtitleLabel {
    color: #64748B;
    font-size: 12px;
}

QLabel#titleLabel {
    color: #1E293B;
    font-size: 15px;
    font-weight: 800;
}

QLabel#quantityLabel,
QLabel#recentTitleLabel {
    color: #1E293B;
    font-weight: 800;
}

QLineEdit#barcodeInput {
    background-color: #FFFFFF;
    border: 1px solid #4F46E5;
    border-top-left-radius: 8px;
    border-bottom-left-radius: 8px;
    border-top-right-radius: 0px;
    border-bottom-right-radius: 0px;
    color: #1E293B;
    padding: 0px 12px;
    min-height: 44px;
    max-height: 44px;
}

QLabel#barcodeIconLabel {
    background-color: #FFFFFF;
    border: 1px solid #4F46E5;
    border-left: none;
    border-top-right-radius: 8px;
    border-bottom-right-radius: 8px;
    color: #4F46E5;
    font-size: 18px;
}

QPushButton#minusButton,
QPushButton#plusButton {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    color: #1E293B;
    font-size: 16px;
    font-weight: 800;
}

QSpinBox#quantityInput {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-left: none;
    border-right: none;
    color: #1E293B;
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
    color: #334155;
    font-weight: 800;
}

QLabel#recentNameLabel,
QLabel#recentNameLabel_2,
QLabel#recentNameLabel_3,
QLabel#recentDateLabel,
QLabel#recentDateLabel_2,
QLabel#recentDateLabel_3 {
    color: #64748B;
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
    background-color: #0A1020;
}

QWidget#pageCard {
    background-color: transparent;
    border: none;
}

QWidget#scanCard,
QWidget#recentScansCard {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QLabel#pageTitleLabel {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel,
QLabel#subtitleLabel {
    color: #94A3B8;
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
    background-color: #0A1020;
    border: 1px solid #6366F1;
    border-top-left-radius: 8px;
    border-bottom-left-radius: 8px;
    border-top-right-radius: 0px;
    border-bottom-right-radius: 0px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 44px;
    max-height: 44px;
}

QLabel#barcodeIconLabel {
    background-color: #0A1020;
    border: 1px solid #6366F1;
    border-left: none;
    border-top-right-radius: 8px;
    border-bottom-right-radius: 8px;
    color: #93C5FD;
    font-size: 18px;
}

QPushButton#minusButton,
QPushButton#plusButton {
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
    color: #CBD5E1;
    font-size: 16px;
    font-weight: 800;
}

QSpinBox#quantityInput {
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
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
    color: #CBD5E1;
    font-weight: 800;
}

QLabel#recentNameLabel,
QLabel#recentNameLabel_2,
QLabel#recentNameLabel_3,
QLabel#recentDateLabel,
QLabel#recentDateLabel_2,
QLabel#recentDateLabel_3 {
    color: #94A3B8;
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
    background-color: #F8FAFC;
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
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QLineEdit#itemSearchInput {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #1E293B;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addItemButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QPushButton#filterItemButton,
QPushButton#itemSearchButton {
    background-color: #FFFFFF;
    color: #1E293B;
    border: 1px solid #E2E8F0;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #475569;
}

QPushButton#activePageButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#ItemsPageClass {
    background-color: #0A1020;
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
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #94A3B8;
    font-size: 12px;
}

QLineEdit#itemSearchInput {
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addItemButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QPushButton#filterItemButton,
QPushButton#itemSearchButton {
    background-color: #121C30;
    color: #FFFFFF;
    border: 1px solid #2E3D5B;
}

QWidget#paginationContainer {
    background-color: #121C30;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #4338CA;
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
    background-color: #F8FAFC;
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
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QLineEdit#assetSearchInput {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #1E293B;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addAssetButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QPushButton#filterAssetButton,
QPushButton#assetSearchButton {
    background-color: #FFFFFF;
    color: #1E293B;
    border: 1px solid #E2E8F0;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #475569;
}

QPushButton#activePageButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#AssetsPageClass {
    background-color: #0A1020;
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
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #94A3B8;
    font-size: 12px;
}

QLineEdit#assetSearchInput {
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addAssetButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QPushButton#filterAssetButton,
QPushButton#assetSearchButton {
    background-color: #121C30;
    color: #FFFFFF;
    border: 1px solid #2E3D5B;
}

QWidget#paginationContainer {
    background-color: #121C30;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #4338CA;
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
    background-color: #F8FAFC;
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
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #1E293B;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QComboBox#roleFilter {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #1E293B;
    padding: 0px 10px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addUserButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #475569;
}

QPushButton#activePageButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#UsersPageClass {
    background-color: #0A1020;
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
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #94A3B8;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QComboBox#roleFilter {
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 10px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addUserButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #121C30;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #4338CA;
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
    background-color: #F8FAFC;
    color: #1E293B;
}

QLabel {
    background-color: transparent;
    border: none;
    color: #1E293B;
}

QLabel#pageTitle {
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#subtitleLabel {
    color: #64748B;
    font-size: 12px;
}

QWidget#metricCard1,
QWidget#metricCard2,
QWidget#metricCard3,
QWidget#metricCard4,
QWidget#itemSummaryCard,
QWidget#assetSummaryCard,
QWidget#recentItemsCard,
QWidget#lowStockCard {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QWidget#miniSummaryBox1,
QWidget#miniSummaryBox2,
QWidget#miniSummaryBox3,
QWidget#miniSummaryBox4 {
    background-color: #F8FAFC;
    border: 1px solid #E2E8F0;
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
    color: #64748B;
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
    color: #1E293B;
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
    color: #DC2626;
    font-size: 12px;
    font-weight: 700;
}

QLabel#cardTitle1,
QLabel#cardTitle2,
QLabel#cardTitle3,
QLabel#cardTitle4 {
    color: #1E293B;
    font-size: 16px;
    font-weight: 800;
}

QWidget#blueIconBox,
QWidget#blueIconBox2 {
    background-color: #EEF2FF;
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
    color: #4F46E5;
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
    color: #4F46E5;
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
    color: #DC2626;
    font-size: 18px;
}

QPushButton#viewAllItemsButton,
QPushButton#viewAllLowStockButton {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    color: #4F46E5;
    border-radius: 8px;
    padding: 5px 10px;
    font-weight: 700;
}

QPushButton#viewAllItemsButton:hover,
QPushButton#viewAllLowStockButton:hover {
    background-color: #EEF2FF;
}

QTableWidget {
    background-color: transparent;
    border: none;
    color: #1E293B;
    gridline-color: transparent;
    outline: none;
}

QTableWidget::item {
    background-color: transparent;
    border-bottom: 1px solid #E2E8F0;
    padding-left: 8px;
}

QHeaderView::section {
    background-color: transparent;
    color: #64748B;
    border: none;
    border-bottom: 1px solid #E2E8F0;
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
    background-color: #0A1020;
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
    color: #94A3B8;
    font-size: 12px;
}

QWidget#metricCard1,
QWidget#metricCard2,
QWidget#metricCard3,
QWidget#metricCard4,
QWidget#itemSummaryCard,
QWidget#assetSummaryCard,
QWidget#recentItemsCard,
QWidget#lowStockCard {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QWidget#miniSummaryBox1,
QWidget#miniSummaryBox2,
QWidget#miniSummaryBox3,
QWidget#miniSummaryBox4 {
    background-color: #121C30;
    border: 1px solid #1F2E45;
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
    color: #94A3B8;
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
    color: #F87171;
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
    background-color: #211C54;
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
    color: #93C5FD;
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
    color: #6366F1;
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
    color: #EF4444;
    font-size: 18px;
}

QPushButton#viewAllItemsButton,
QPushButton#viewAllLowStockButton {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    color: #818CF8;
    border-radius: 8px;
    padding: 5px 10px;
    font-weight: 700;
}

QPushButton#viewAllItemsButton:hover,
QPushButton#viewAllLowStockButton:hover {
    background-color: #1B2843;
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
    border-bottom: 1px solid #1C273F;
    padding-left: 8px;
}

QHeaderView::section {
    background-color: transparent;
    color: #94A3B8;
    border: none;
    border-bottom: 1px solid #1F2E45;
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
    background-color: #F8FAFC;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #1E293B;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addTruckButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #475569;
}

QPushButton#activePageButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#TrucksPageClass {
    background-color: #0A1020;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #94A3B8;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addTruckButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #121C30;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #4338CA;
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
    background-color: #F8FAFC;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #1E293B;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#newTemplateButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #475569;
}

QPushButton#activePageButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#StockTemplatesPageClass {
    background-color: #0A1020;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #94A3B8;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#newTemplateButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #121C30;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #4338CA;
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
    background-color: #F8FAFC;
}

QWidget#pageContainer,
QWidget#toolbarContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QPushButton#assignTemplateButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #475569;
}

QPushButton#activePageButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#AssignmentsPageClass {
    background-color: #0A1020;
}

QWidget#pageContainer,
QWidget#toolbarContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #94A3B8;
    font-size: 12px;
}

QPushButton#assignTemplateButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #121C30;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #4338CA;
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
    background-color: #F8FAFC;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #1E293B;
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
    background-color: #0A1020;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #94A3B8;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    color: #CBD5E1;
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
    background-color: #F8FAFC;
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
    border: 1px solid #E2E8F0;
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
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    background-color: transparent;
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    background-color: transparent;
    color: #64748B;
    font-size: 12px;
}

QLabel#metricTitle,
QLabel#metricTitle1,
QLabel#metricTitle2 {
    color: #64748B;
    font-size: 12px;
    font-weight: 700;
}

QLabel#metricValue {
    color: #DC2626;
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
    color: #DC2626;
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
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #1E293B;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#exportButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #FFFFFF;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #475569;
}

QPushButton#activePageButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#LowStockAlertsPageClass {
    background-color: #0A1020;
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
    background-color: #121C30;
    border: 1px solid #2E3D5B;
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
    background-color: #121C30;
    border: 1px solid #2E3D5B;
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
    color: #94A3B8;
    font-size: 12px;
}

QLabel#metricTitle,
QLabel#metricTitle1,
QLabel#metricTitle2 {
    color: #94A3B8;
    font-size: 12px;
    font-weight: 700;
}

QLabel#metricValue {
    color: #EF4444;
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
    color: #F87171;
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
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#exportButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #121C30;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #4338CA;
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
    background-color: #F8FAFC;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QLineEdit#searchInput,
QComboBox#statusFilter {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #1E293B;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#uploadButton {
    background-color: #4F46E5;
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
    background-color: #E5E7EB;
    border: 1px solid #D1D5DB;
    color: #9CA3AF;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#ReceiptsPageClass {
    background-color: #0A1020;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QLabel#pageTitle {
    color: #FFFFFF;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #94A3B8;
    font-size: 12px;
}

QLineEdit#searchInput,
QComboBox#statusFilter {
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#uploadButton {
    background-color: #4F46E5;
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
    background-color: #111827;
    border: 1px solid #374151;
    color: #6B7280;
}

)";
}

QString Theme::notificationDialogStyle(AppTheme theme)
{
    QString background =
        theme == AppTheme::Light ? "#F8FAFC" : "#0F172A";

    QString card =
        theme == AppTheme::Light ? "#FFFFFF" : "#111827";

    QString text =
        theme == AppTheme::Light ? "#0F172A" : "#E5E7EB";

    QString muted =
        theme == AppTheme::Light ? "#64748B" : "#9CA3AF";

    QString border =
        theme == AppTheme::Light ? "#E2E8F0" : "#1E2942";

    QString button =
        theme == AppTheme::Light ? "#4F46E5" : "#6366F1";

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
            background-color: #4338CA;
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
    background-color: #F8FAFC;
    color: #1E293B;
}

QWidget#pageCard, QWidget#card, QWidget#tableCard, QWidget#toolbarCard, QWidget#formCard, QWidget#summaryCard,
QFrame#pageCard, QFrame#card, QFrame#tableCard, QFrame#toolbarCard, QFrame#formCard, QFrame#summaryCard {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel { color: #1E293B; }

QLabel#titleLabel, QLabel#pageTitleLabel, QLabel#sectionTitleLabel {
    color: #1E293B;
    font-weight: 800;
}

QLabel#subtitleLabel, QLabel#breadcrumbLabel, QLabel#descriptionLabel {
    color: #64748B;
}

QPushButton#addItemButton, QPushButton#addAssetButton, QPushButton#addButton, QPushButton#newButton,
QPushButton#saveButton, QPushButton#saveTruckButton, QPushButton#saveProfileButton,
QPushButton#applyThemeButton, QPushButton#uploadReceiptButton, QPushButton#assignTemplateButton,
QPushButton#submitButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QPushButton#deleteButton, QPushButton#logoutButton {
    background-color: #DC2626;
    color: #FFFFFF;
    border: none;
}

QPushButton#cancelButton {
    background-color: #FFFFFF;
    color: #1E293B;
    border: 1px solid #E2E8F0;
}

QTableWidget {
    background-color: transparent;
    border: none;
    color: #1E293B;
    gridline-color: transparent;
    outline: none;
}

QTableWidget::item {
    background-color: transparent;
    border-bottom: 1px solid #E2E8F0;
    padding-left: 12px;
}

QTableWidget::item:selected {
    background-color: #EEF2FF;
}

QHeaderView::section {
    background-color: #F1F5F9;
    color: #64748B;
    border: none;
    border-bottom: 1px solid #E2E8F0;
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
QPushButton#editButton {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    color: #334155;
    border-radius: 6px;
    padding: 4px 7px;
}

QPushButton#viewButton:hover,
QPushButton#editButton:hover {
    background-color: #4F46E5;
    color: #FFFFFF;
}

QPushButton#deleteButton {
    background-color: #FEE2E2;
    border: 1px solid #FCA5A5;
    color: #DC2626;
    border-radius: 6px;
    padding: 4px 7px;
}

QPushButton#deleteButton:hover {
    background-color: #DC2626;
    color: #FFFFFF;
}

)";
    }

    return R"(
QWidget {
    background-color: #0A1020;
    color: #FFFFFF;
}

QWidget#pageCard, QWidget#card, QWidget#tableCard, QWidget#toolbarCard, QWidget#formCard, QWidget#summaryCard,
QFrame#pageCard, QFrame#card, QFrame#tableCard, QFrame#toolbarCard, QFrame#formCard, QFrame#summaryCard {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 14px;
}

QLabel { color: #FFFFFF; }

QLabel#titleLabel, QLabel#pageTitleLabel, QLabel#sectionTitleLabel {
    color: #FFFFFF;
    font-weight: 800;
}

QLabel#subtitleLabel, QLabel#breadcrumbLabel, QLabel#descriptionLabel {
    color: #94A3B8;
}

QPushButton#addItemButton, QPushButton#addAssetButton, QPushButton#addButton, QPushButton#newButton,
QPushButton#saveButton, QPushButton#saveTruckButton, QPushButton#saveProfileButton,
QPushButton#applyThemeButton, QPushButton#uploadReceiptButton, QPushButton#assignTemplateButton,
QPushButton#submitButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
}

QPushButton#deleteButton, QPushButton#logoutButton {
    background-color: #DC2626;
    color: #FFFFFF;
    border: none;
}

QPushButton#cancelButton {
    background-color: #121C30;
    color: #FFFFFF;
    border: 1px solid #2E3D5B;
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
    border-bottom: 1px solid #1C273F;
    padding-left: 12px;
}

QTableWidget::item:selected {
    background-color: #1C273F;
}

QHeaderView::section {
    background-color: #0C1728;
    color: #94A3B8;
    border: none;
    border-bottom: 1px solid #1F2E45;
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
QPushButton#editButton {
    background-color: #1B2843;
    border: 1px solid #2E3D5B;
    color: #CBD5E1;
    border-radius: 6px;
    padding: 4px 7px;
}

QPushButton#viewButton:hover,
QPushButton#editButton:hover {
    background-color: #4338CA;
    color: #FFFFFF;
}

QPushButton#deleteButton {
    background-color: #3B1115;
    border: 1px solid #7F1D1D;
    color: #F87171;
    border-radius: 6px;
    padding: 4px 7px;
}

QPushButton#deleteButton:hover {
    background-color: #DC2626;
    color: #FFFFFF;
}

)";
}

QString Theme::dialogStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(

QDialog {
    background-color: transparent;
    color: #1E293B;
    font-family: "Segoe UI";
    font-size: 13px;
}

QWidget#dialogCard {
    background-color: #FFFFFF;
    border: none;
    border-radius: 14px;
}

QWidget#headerContainer {
    background-color: #4F46E5;
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
    color: #1E293B;
    font-weight: 600;
}

QLabel#titleLabel {
    color: #FFFFFF;
    font-size: 18px;
    font-weight: 700;
}

QLabel#statusLabel {
    color: #DC2626;
    font-weight: 700;
}

QLineEdit,
QComboBox,
QSpinBox,
QTextEdit,
QPlainTextEdit {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 6px;
    color: #1E293B;
    padding: 4px 10px;
    min-height: 28px;
    max-height: 32px;
}

QLineEdit:focus,
QComboBox:focus,
QSpinBox:focus,
QTextEdit:focus,
QPlainTextEdit:focus {
    border: 1px solid #4F46E5;
}

QPushButton {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 6px;
    color: #1E293B;
    padding: 6px 16px;
    font-weight: 600;
    min-width: 110px;
    min-height: 36px;
    max-height: 36px;
}

QPushButton:hover {
    background-color: #F1F5F9;
}

QPushButton#saveButton {
    background-color: #4F46E5;
    border: none;
    color: #FFFFFF;
}

QPushButton#saveButton:hover {
    background-color: #4338CA;
}

QPushButton#cancelButton {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    color: #1E293B;
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
    background-color: #121C30;
    border: none;
    border-radius: 14px;
}

QWidget#headerContainer {
    background-color: #4338CA;
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

QLabel#statusLabel {
    color: #F87171;
    font-weight: 700;
}

QLineEdit,
QComboBox,
QSpinBox,
QTextEdit,
QPlainTextEdit {
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
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
    border: 1px solid #6366F1;
}

QPushButton {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 6px;
    color: #FFFFFF;
    padding: 6px 16px;
    font-weight: 600;
    min-width: 110px;
    min-height: 36px;
    max-height: 36px;
}

QPushButton:hover {
    background-color: #1B2843;
}

QPushButton#saveButton {
    background-color: #4F46E5;
    border: none;
    color: #FFFFFF;
}

QPushButton#saveButton:hover {
    background-color: #4338CA;
}

QPushButton#cancelButton {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
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
    background-color: #F8FAFC;
    color: #1E293B;
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
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel {
    background-color: transparent;
    border: none;
    color: #1E293B;
}

QLabel#pageTitle,
QLabel#titleLabel,
QLabel#cardTitle1,
QLabel#cardTitle2,
QLabel#cardTitle3,
QLabel#cardTitle4 {
    background-color: transparent;
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel,
QLabel#subtitleLabel {
    background-color: transparent;
    color: #64748B;
    font-size: 12px;
}

QLineEdit#searchInput,
QComboBox#statusFilter,
QComboBox#roleFilter {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    color: #1E293B;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QTableWidget,
QTableView {
    background-color: #FFFFFF;
    border: none;
    color: #1E293B;
    gridline-color: transparent;
    selection-background-color: #EEF2FF;
    selection-color: #1E293B;
}

QTableWidget::item,
QTableView::item {
    background-color: #FFFFFF;
    color: #1E293B;
    border-bottom: 1px solid #E2E8F0;
    padding-left: 8px;
}

QTableWidget::item:selected,
QTableView::item:selected {
    background-color: #EEF2FF;
    color: #1E293B;
}

QHeaderView::section {
    background-color: #FFFFFF;
    color: #64748B;
    border: none;
    border-bottom: 1px solid #E2E8F0;
    padding-left: 8px;
    font-size: 12px;
    font-weight: 700;
}

QPushButton#addTruckButton,
QPushButton#newTemplateButton,
QPushButton#assignTemplateButton,
QPushButton#exportButton,
QPushButton#uploadButton {
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    font-weight: 800;
}

QPushButton#viewButton {
    background-color: #EEF2FF;
    border: 1px solid #93C5FD;
    color: #4F46E5;
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
    background-color: #FEE2E2;
    border: 1px solid #FCA5A5;
    color: #DC2626;
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
    color: #475569;
}

QPushButton#activePageButton {
    background-color: #4F46E5;
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
    background-color: #0A1020;
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
    background-color: #121C30;
    border: 1px solid #2E3D5B;
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
    color: #94A3B8;
    font-size: 12px;
}

QLineEdit#searchInput,
QComboBox#statusFilter,
QComboBox#roleFilter {
    background-color: #0A1020;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QTableWidget,
QTableView {
    background-color: #121C30;
    border: none;
    color: #FFFFFF;
    gridline-color: transparent;
    selection-background-color: #4338CA;
    selection-color: #FFFFFF;
}

QTableWidget::item,
QTableView::item {
    background-color: #121C30;
    color: #FFFFFF;
    border-bottom: 1px solid #1C273F;
    padding-left: 8px;
}

QHeaderView::section {
    background-color: #121C30;
    color: #94A3B8;
    border: none;
    border-bottom: 1px solid #1F2E45;
    padding-left: 8px;
    font-size: 12px;
    font-weight: 700;
}

QWidget#paginationContainer {
    background-color: #121C30;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #4338CA;
    color: #FFFFFF;
    border: none;
}

)";
}


QString Theme::reportsPageStyle(AppTheme theme)
{
    return dataPageStyle(theme) + (theme == AppTheme::Light ? R"(

QWidget#ReportsPageClass {
    background-color: #F8FAFC;
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
    border: 1px solid #E2E8F0;
    border-radius: 14px;
}

QLabel#pageTitle,
QLabel#titleLabel {
    color: #1E293B;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel,
QLabel#subtitleLabel {
    color: #64748B;
    font-size: 12px;
}

QPushButton#inventorySummaryBtn,
QPushButton#assetsSummaryBtn,
QPushButton#refreshBtn {
    background-color: #FFFFFF;
    color: #1E293B;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 800;
}

QPushButton#inventorySummaryBtn:hover,
QPushButton#assetsSummaryBtn:hover,
QPushButton#refreshBtn:hover {
    background-color: #EFF6FF;
    border-color: #4F46E5;
}

QPushButton#exportPdfBtn {
    background-color: #DC2626;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 800;
}

QPushButton#exportPdfBtn:hover {
    background-color: #EF4444;
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
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 800;
}

QPushButton#exportCsvBtn:hover {
    background-color: #6366F1;
}

QTableWidget#reportTable,
QTableWidget {
    background-color: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 10px;
    color: #1E293B;
    gridline-color: #E5E7EB;
    selection-background-color: #EEF2FF;
    selection-color: #1E293B;
    alternate-background-color: #F8FAFC;
}

QTableWidget::item {
    padding: 8px;
    border-bottom: 1px solid #E5E7EB;
}

QHeaderView::section {
    background-color: #E2E8F0;
    color: #334155;
    border: none;
    border-bottom: 1px solid #E2E8F0;
    padding: 8px;
    font-weight: 800;
}

)" : R"(

QWidget#ReportsPageClass {
    background-color: #0A1020;
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
    background-color: #121C30;
    border: 1px solid #2E3D5B;
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
    color: #94A3B8;
    font-size: 12px;
}

QPushButton#inventorySummaryBtn,
QPushButton#assetsSummaryBtn,
QPushButton#refreshBtn {
    background-color: #121C30;
    color: #F8FAFC;
    border: 1px solid #2E3D5B;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 800;
}

QPushButton#inventorySummaryBtn:hover,
QPushButton#assetsSummaryBtn:hover,
QPushButton#refreshBtn:hover {
    background-color: #1B2843;
    border-color: #6366F1;
}

QPushButton#exportPdfBtn {
    background-color: #DC2626;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 800;
}

QPushButton#exportPdfBtn:hover {
    background-color: #EF4444;
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
    background-color: #4F46E5;
    color: #FFFFFF;
    border: none;
    border-radius: 8px;
    padding: 8px 12px;
    font-weight: 800;
}

QPushButton#exportCsvBtn:hover {
    background-color: #6366F1;
}

QTableWidget#reportTable,
QTableWidget {
    background-color: #121C30;
    border: 1px solid #2E3D5B;
    border-radius: 10px;
    color: #F8FAFC;
    gridline-color: #1E2942;
    selection-background-color: #4338CA;
    selection-color: #FFFFFF;
    alternate-background-color: #152138;
}

QTableWidget::item {
    padding: 8px;
    border-bottom: 1px solid #1C273F;
}

QHeaderView::section {
    background-color: #152138;
    color: #CBD5E1;
    border: none;
    border-bottom: 1px solid #2E3D5B;
    padding: 8px;
    font-weight: 800;
}

)");
}
