#include "Theme.h"

#include <QApplication>

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

    widget->setStyleSheet(loginStyle(AppTheme::Dark));
}

void Theme::applyTheme(AppTheme theme)
{
    qApp->setStyleSheet(theme == AppTheme::Light ? lightStyleSheet() : darkStyleSheet());
}

QString Theme::globalStyleSheet()
{
    return darkStyleSheet();
}

QString Theme::darkStyleSheet()
{
    return R"(
* { font-family: "Segoe UI"; font-size: 13px; }

QMainWindow, QWidget {
    background-color: #050B14;
    color: #F8FAFC;
}

QLabel {
    color: #F8FAFC;
    background: transparent;
}

QLineEdit, QComboBox, QSpinBox, QTextEdit, QPlainTextEdit {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 8px;
    color: #F8FAFC;
    padding: 8px 10px;
    selection-background-color: #2563EB;
}

QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QTextEdit:focus, QPlainTextEdit:focus {
    border: 1px solid #3B82F6;
}

QPushButton {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 8px;
    color: #F8FAFC;
    padding: 8px 12px;
    font-weight: 700;
}

QPushButton:hover {
    background-color: #172033;
    border-color: #3B82F6;
}

QPushButton:checked {
    background-color: #1D4ED8;
    color: #FFFFFF;
}

QPushButton:disabled {
    color: #64748B;
    background-color: #111827;
    border-color: #1E293B;
}

QTableWidget, QTreeWidget, QListWidget {
    background-color: #0B1626;
    border: 1px solid #263A57;
    color: #F8FAFC;
    gridline-color: #1F2937;
    selection-background-color: #1D4ED8;
    selection-color: #FFFFFF;
    alternate-background-color: #101B2C;
}

QHeaderView::section {
    background-color: #101B2C;
    color: #CBD5E1;
    border: none;
    border-bottom: 1px solid #263A57;
    padding: 8px;
    font-weight: 700;
}

QFrame { background: transparent; }

QScrollBar:vertical {
    background: transparent;
    width: 8px;
    margin: 4px;
}
QScrollBar::handle:vertical {
    background: #334155;
    border-radius: 4px;
    min-height: 32px;
}
QScrollBar::handle:vertical:hover { background: #475569; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }

QToolTip {
    background-color: #0B1626;
    color: #FFFFFF;
    border: 1px solid #3B82F6;
}
)";
}

QString Theme::lightStyleSheet()
{
    return R"(
* { font-family: "Segoe UI"; font-size: 13px; }

QMainWindow, QWidget {
    background-color: #F4F7FB;
    color: #111827;
}

QLabel {
    color: #111827;
    background: transparent;
}

QLineEdit, QComboBox, QSpinBox, QTextEdit, QPlainTextEdit {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 8px;
    color: #111827;
    padding: 8px 10px;
    selection-background-color: #2563EB;
}

QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QTextEdit:focus, QPlainTextEdit:focus {
    border: 1px solid #2563EB;
}

QPushButton {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 8px;
    color: #111827;
    padding: 8px 12px;
    font-weight: 700;
}

QPushButton:hover {
    background-color: #EFF6FF;
    border-color: #2563EB;
}

QPushButton:checked {
    background-color: #2563EB;
    color: #FFFFFF;
}

QPushButton:disabled {
    color: #94A3B8;
    background-color: #F1F5F9;
    border-color: #CBD5E1;
}

QTableWidget, QTreeWidget, QListWidget {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    color: #111827;
    gridline-color: #E5E7EB;
    selection-background-color: #DBEAFE;
    selection-color: #111827;
    alternate-background-color: #F8FAFC;
}

QHeaderView::section {
    background-color: #E2E8F0;
    color: #334155;
    border: none;
    border-bottom: 1px solid #CBD5E1;
    padding: 8px;
    font-weight: 700;
}

QFrame { background: transparent; }

QScrollBar:vertical {
    background: transparent;
    width: 8px;
    margin: 4px;
}
QScrollBar::handle:vertical {
    background: #CBD5E1;
    border-radius: 4px;
    min-height: 32px;
}
QScrollBar::handle:vertical:hover { background: #94A3B8; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }

QToolTip {
    background-color: #FFFFFF;
    color: #111827;
    border: 1px solid #2563EB;
}
)";
}

QString Theme::loginStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QMainWindow, QWidget {
    background-color: #F4F7FB;
    color: #111827;
    font-family: "Segoe UI";
}

QFrame, QWidget#loginCard, QWidget#card {
    background-color: transparent;
    border: none;
    border-radius: 0px;
}

QLabel { color: #111827; background: transparent; }

QLineEdit {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 8px;
    color: #111827;
    padding: 8px 12px;
    min-height: 36px;
}
QLineEdit:focus { border: 1px solid #2563EB; }

QPushButton#loginButton, QPushButton {
    background-color: #2563EB;
    border: none;
    border-radius: 8px;
    color: #FFFFFF;
    padding: 10px 14px;
    font-weight: 800;
    min-height: 38px;
}
QPushButton#loginButton:hover, QPushButton:hover { background-color: #3B82F6; }

QCheckBox { color: #475569; }

QPushButton#forgotPasswordButton {
    background: transparent;
    border: none;
    color: #2563EB;
    padding: 0px;
    min-height: 0px;
    text-align: left;
}

QLabel#statusLabel { color: #CA8A04; font-weight: 700; }
)";
    }

    return R"(
QMainWindow, QWidget {
    background-color: #050B14;
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
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 8px;
    color: #F8FAFC;
    padding: 8px 12px;
    min-height: 36px;
}
QLineEdit:focus { border: 1px solid #3B82F6; }

QPushButton#loginButton, QPushButton {
    background-color: #2563EB;
    border: none;
    border-radius: 8px;
    color: #FFFFFF;
    padding: 10px 14px;
    font-weight: 800;
    min-height: 38px;
}
QPushButton#loginButton:hover, QPushButton:hover { background-color: #3B82F6; }

QCheckBox { color: #CBD5E1; }

QPushButton#forgotPasswordButton {
    background: transparent;
    border: none;
    color: #60A5FA;
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
    background-color: #F4F7FB;
}
QWidget#sidebarContainer {
    background-color: #FFFFFF;
    border-right: 1px solid #CBD5E1;
}
QWidget#verticalContainer {
    background-color: #F4F7FB;
}
)";
    }

    return R"(
QMainWindow#DashboardWindow, QWidget#centralWidget, QStackedWidget#mainStack {
    background-color: #050B14;
}
QWidget#sidebarContainer {
    background-color: #08111F;
    border-right: 1px solid #1E293B;
}
QWidget#verticalContainer {
    background-color: #050B14;
}
)";
}

QString Theme::sidebarStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#SidebarWidgetClass { background-color: #FFFFFF; }

QLabel#logoLabel {
    font-size: 18px;
    font-weight: 800;
    color: #111827;
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
    padding: 10px 12px;
    border-radius: 8px;
    border: none;
    background: transparent;
    color: #111827;
    font-weight: 650;
}
QPushButton:hover { background-color: #EFF6FF; }
QPushButton:checked { background-color: #2563EB; color: #FFFFFF; }

QFrame#userCard {
    background-color: #F8FAFC;
    border: 1px solid #CBD5E1;
    border-radius: 10px;
}

QLabel#avatarLabel {
    background-color: #2563EB;
    border-radius: 14px;
    color: #FFFFFF;
    font-weight: 800;
}

QLabel#userNameLabel {
    font-size: 12px;
    font-weight: 800;
    color: #111827;
}

QLabel#userRoleLabel {
    font-size: 10px;
    color: #64748B;
}

QPushButton#logoutButton {
    background: transparent;
    border: none;
    color: #DC2626;
}
QPushButton#logoutButton:hover { background-color: #FEE2E2; }
)";
    }

    return R"(
QWidget#SidebarWidgetClass { background-color: #08111F; }

QLabel#logoLabel {
    font-size: 18px;
    font-weight: 800;
    color: #FFFFFF;
}

QLabel#sectionLabel, QLabel#sectionLabel2 {
    color: #94A3B8;
    font-size: 11px;
    font-weight: 800;
}

QFrame#lineDivider, QFrame#lineDivider2 {
    color: #1E293B;
    background-color: #1E293B;
    max-height: 1px;
}

QPushButton {
    text-align: left;
    padding: 10px 12px;
    border-radius: 8px;
    border: none;
    background: transparent;
    color: #F8FAFC;
    font-weight: 650;
}
QPushButton:hover { background-color: #172033; }
QPushButton:checked { background-color: #1D4ED8; color: #FFFFFF; }

QFrame#userCard {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 10px;
}

QLabel#avatarLabel {
    background-color: #1D4ED8;
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
    color: #94A3B8;
}

QPushButton#logoutButton {
    background: transparent;
    border: none;
    color: #F87171;
}
QPushButton#logoutButton:hover { background-color: #3B1115; }
)";
}

QString Theme::verticalBarStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#VerticalWidget { background-color: #F4F7FB; }

QLabel#welcomeLabel {
    color: #111827;
    font-size: 22px;
    font-weight: 800;
}
QLabel#subTitleLabel {
    color: #64748B;
    font-size: 12px;
}

QFrame#searchContainer {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 10px;
}

QLineEdit#searchInput {
    background: transparent;
    border: none;
    color: #111827;
    padding: 0px;
}

QLabel#searchIcon {
    color: #64748B;
    font-size: 14px;
}

QLabel#shortcutLabel {
    background-color: #E2E8F0;
    color: #475569;
    border-radius: 6px;
    padding: 4px 8px;
    font-size: 11px;
}

QPushButton#notificationButton {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 17px;
}
QPushButton#notificationButton:hover { background-color: #EFF6FF; }

QLabel#avatarLabel {
    background-color: #2563EB;
    color: #FFFFFF;
    border-radius: 17px;
    font-weight: 800;
}
)";
    }

    return R"(
QWidget#VerticalWidget { background-color: #050B14; }

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
    background-color: #0B1626;
    border: 1px solid #263A57;
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
    background-color: #172033;
    color: #94A3B8;
    border-radius: 6px;
    padding: 4px 8px;
    font-size: 11px;
}

QPushButton#notificationButton {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 17px;
}
QPushButton#notificationButton:hover { background-color: #172033; }

QLabel#avatarLabel {
    background-color: #1D4ED8;
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
QWidget#DashboardPageClass { background-color: #F4F7FB; }

QWidget#pageCard, QWidget#summaryCard,
QFrame#totalItemsCard, QFrame#totalAssetsCard, QFrame#quantityInHandCard, QFrame#receivedCard,
QFrame#itemSummaryCard, QFrame#assetSummaryCard, QFrame#recentItemsCard, QFrame#lowStockCard,
QWidget#totalItemsCard, QWidget#totalAssetsCard, QWidget#quantityInHandCard, QWidget#receivedCard,
QWidget#itemSummaryCard, QWidget#assetSummaryCard, QWidget#recentItemsCard, QWidget#lowStockCard {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 12px;
}

QLabel { color: #111827; }

QLabel#totalItemsTitle, QLabel#totalAssetsTitle, QLabel#quantityTitle, QLabel#receivedTitle,
QLabel#itemLeftLabel, QLabel#itemRightLabel, QLabel#assetActiveLabel, QLabel#assetInactiveLabel {
    color: #64748B;
    font-size: 12px;
}

QLabel#totalItemsValue, QLabel#totalAssetsValue, QLabel#quantityInHandValue, QLabel#toBeReceivedValue,
QLabel#itemQuantityValue, QLabel#itemReceivedValue, QLabel#assetActiveValue, QLabel#assetInactiveValue {
    color: #111827;
    font-weight: 800;
}

QLabel#itemSummaryTitle, QLabel#assetSummaryTitle, QLabel#recentTitle, QLabel#lowStockTitle {
    color: #111827;
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
    background-color: #DBEAFE;
    color: #2563EB;
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

QLabel#itemChartBlue { color: #2563EB; }
QLabel#itemChartOrange { color: #F97316; }
QLabel#assetChartGreen { color: #16A34A; }
QLabel#assetChartRed { color: #DC2626; }
)";
    }

    return R"(
QWidget#DashboardPageClass { background-color: #050B14; }

QWidget#pageCard, QWidget#summaryCard,
QFrame#totalItemsCard, QFrame#totalAssetsCard, QFrame#quantityInHandCard, QFrame#receivedCard,
QFrame#itemSummaryCard, QFrame#assetSummaryCard, QFrame#recentItemsCard, QFrame#lowStockCard,
QWidget#totalItemsCard, QWidget#totalAssetsCard, QWidget#quantityInHandCard, QWidget#receivedCard,
QWidget#itemSummaryCard, QWidget#assetSummaryCard, QWidget#recentItemsCard, QWidget#lowStockCard {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 12px;
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
    background-color: #123A78;
    color: #3B82F6;
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

QLabel#itemChartBlue { color: #3B82F6; }
QLabel#itemChartOrange { color: #F97316; }
QLabel#assetChartGreen { color: #22C55E; }
QLabel#assetChartRed { color: #EF4444; }
)";
}

QString Theme::settingsPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget#SettingsPageClass { background-color: #F4F7FB; }

QWidget#profileCard, QWidget#appearanceCard, QWidget#sessionCard {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 12px;
}

QLabel#titleLabel {
    color: #111827;
    font-size: 22px;
    font-weight: 800;
}
QLabel#subtitleLabel {
    color: #64748B;
    font-size: 13px;
}
QLabel#profileTitleLabel, QLabel#appearanceTitleLabel, QLabel#sessionTitleLabel {
    color: #111827;
    font-size: 16px;
    font-weight: 800;
}
QLabel#roleValueLabel {
    color: #2563EB;
    font-weight: 800;
}
QPushButton#saveProfileButton, QPushButton#applyThemeButton {
    background-color: #2563EB;
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
QWidget#SettingsPageClass { background-color: #050B14; }

QWidget#profileCard, QWidget#appearanceCard, QWidget#sessionCard {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 12px;
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
    background-color: #2563EB;
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
    background-color: #F4F7FB;
}

QWidget#pageCard {
    background-color: transparent;
    border: none;
}

QWidget#scanCard,
QWidget#recentScansCard {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 12px;
}

QLabel#pageTitleLabel {
    color: #111827;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel,
QLabel#subtitleLabel {
    color: #64748B;
    font-size: 12px;
}

QLabel#titleLabel {
    color: #111827;
    font-size: 15px;
    font-weight: 800;
}

QLabel#quantityLabel,
QLabel#recentTitleLabel {
    color: #111827;
    font-weight: 800;
}

QLineEdit#barcodeInput {
    background-color: #FFFFFF;
    border: 1px solid #2563EB;
    border-top-left-radius: 8px;
    border-bottom-left-radius: 8px;
    border-top-right-radius: 0px;
    border-bottom-right-radius: 0px;
    color: #111827;
    padding: 0px 12px;
    min-height: 44px;
    max-height: 44px;
}

QLabel#barcodeIconLabel {
    background-color: #FFFFFF;
    border: 1px solid #2563EB;
    border-left: none;
    border-top-right-radius: 8px;
    border-bottom-right-radius: 8px;
    color: #2563EB;
    font-size: 18px;
}

QPushButton#minusButton,
QPushButton#plusButton {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    color: #111827;
    font-size: 16px;
    font-weight: 800;
}

QSpinBox#quantityInput {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-left: none;
    border-right: none;
    color: #111827;
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
    background-color: #050B14;
}

QWidget#pageCard {
    background-color: transparent;
    border: none;
}

QWidget#scanCard,
QWidget#recentScansCard {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 12px;
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
    background-color: #050B14;
    border: 1px solid #3B82F6;
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
    background-color: #050B14;
    border: 1px solid #3B82F6;
    border-left: none;
    border-top-right-radius: 8px;
    border-bottom-right-radius: 8px;
    color: #93C5FD;
    font-size: 18px;
}

QPushButton#minusButton,
QPushButton#plusButton {
    background-color: #050B14;
    border: 1px solid #263A57;
    color: #CBD5E1;
    font-size: 16px;
    font-weight: 800;
}

QSpinBox#quantityInput {
    background-color: #050B14;
    border: 1px solid #263A57;
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
    background-color: #F4F7FB;
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
    border: 1px solid #CBD5E1;
    border-radius: 12px;
}

QLabel#pageTitle {
    color: #111827;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QLineEdit#itemSearchInput {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 8px;
    color: #111827;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addItemButton {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

QPushButton#filterItemButton,
QPushButton#itemSearchButton {
    background-color: #FFFFFF;
    color: #111827;
    border: 1px solid #CBD5E1;
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
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#ItemsPageClass {
    background-color: #050B14;
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
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 12px;
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
    background-color: #050B14;
    border: 1px solid #263A57;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addItemButton {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

QPushButton#filterItemButton,
QPushButton#itemSearchButton {
    background-color: #0B1626;
    color: #FFFFFF;
    border: 1px solid #263A57;
}

QWidget#paginationContainer {
    background-color: #0B1626;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #1D4ED8;
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
    background-color: #F4F7FB;
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
    border: 1px solid #CBD5E1;
    border-radius: 12px;
}

QLabel#pageTitle {
    color: #111827;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QLineEdit#assetSearchInput {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 8px;
    color: #111827;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addAssetButton {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

QPushButton#filterAssetButton,
QPushButton#assetSearchButton {
    background-color: #FFFFFF;
    color: #111827;
    border: 1px solid #CBD5E1;
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
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#AssetsPageClass {
    background-color: #050B14;
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
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 12px;
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
    background-color: #050B14;
    border: 1px solid #263A57;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addAssetButton {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

QPushButton#filterAssetButton,
QPushButton#assetSearchButton {
    background-color: #0B1626;
    color: #FFFFFF;
    border: 1px solid #263A57;
}

QWidget#paginationContainer {
    background-color: #0B1626;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #1D4ED8;
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
    background-color: #F4F7FB;
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
    border: 1px solid #CBD5E1;
    border-radius: 12px;
}

QLabel#pageTitle {
    color: #111827;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 8px;
    color: #111827;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QComboBox#roleFilter {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 8px;
    color: #111827;
    padding: 0px 10px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addUserButton {
    background-color: #2563EB;
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
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#UsersPageClass {
    background-color: #050B14;
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
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 12px;
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
    background-color: #050B14;
    border: 1px solid #263A57;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QComboBox#roleFilter {
    background-color: #050B14;
    border: 1px solid #263A57;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 10px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addUserButton {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #0B1626;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #1D4ED8;
    color: #FFFFFF;
    border: none;
}

)";
}

QString Theme::truckDashboardStyle(AppTheme theme) { return truckPageStyle(theme); }

QString Theme::trucksPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return dataPageStyle(theme) + R"(

QWidget#TrucksPageClass {
    background-color: #F4F7FB;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 12px;
}

QLabel#pageTitle {
    color: #111827;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 8px;
    color: #111827;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addTruckButton {
    background-color: #2563EB;
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
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#TrucksPageClass {
    background-color: #050B14;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 12px;
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
    background-color: #050B14;
    border: 1px solid #263A57;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#addTruckButton {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #0B1626;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #1D4ED8;
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
    background-color: #F4F7FB;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 12px;
}

QLabel#pageTitle {
    color: #111827;
    font-size: 20px;
    font-weight: 800;
}

QLabel#breadcrumbLabel {
    color: #64748B;
    font-size: 12px;
}

QLineEdit#searchInput {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 8px;
    color: #111827;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#newTemplateButton {
    background-color: #2563EB;
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
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

)";
    }

    return dataPageStyle(theme) + R"(

QWidget#StockTemplatesPageClass {
    background-color: #050B14;
}

QWidget#pageContainer,
QWidget#toolbarContainer,
QWidget#searchContainer {
    background-color: transparent;
    border: none;
}

QWidget#tableCard {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 12px;
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
    background-color: #050B14;
    border: 1px solid #263A57;
    border-radius: 8px;
    color: #CBD5E1;
    padding: 0px 12px;
    min-height: 38px;
    max-height: 38px;
}

QPushButton#newTemplateButton {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

QWidget#paginationContainer {
    background-color: #0B1626;
    border: none;
}

QLabel#paginationLabel {
    background-color: transparent;
    color: #CBD5E1;
}

QPushButton#activePageButton {
    background-color: #1D4ED8;
    color: #FFFFFF;
    border: none;
}

)";
}

QString Theme::assignmentsPageStyle(AppTheme theme) { return truckPageStyle(theme); }
QString Theme::myTruckStockPageStyle(AppTheme theme) { return truckPageStyle(theme); }
QString Theme::lowStockAlertsPageStyle(AppTheme theme) { return truckPageStyle(theme); }
QString Theme::receiptsPageStyle(AppTheme theme) { return truckPageStyle(theme); }

QString Theme::dataPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget {
    background-color: #F4F7FB;
    color: #111827;
}

QWidget#pageCard, QWidget#card, QWidget#tableCard, QWidget#toolbarCard, QWidget#formCard, QWidget#summaryCard,
QFrame#pageCard, QFrame#card, QFrame#tableCard, QFrame#toolbarCard, QFrame#formCard, QFrame#summaryCard {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 12px;
}

QLabel { color: #111827; }

QLabel#titleLabel, QLabel#pageTitleLabel, QLabel#sectionTitleLabel {
    color: #111827;
    font-weight: 800;
}

QLabel#subtitleLabel, QLabel#breadcrumbLabel, QLabel#descriptionLabel {
    color: #64748B;
}

QPushButton#addItemButton, QPushButton#addAssetButton, QPushButton#addButton, QPushButton#newButton,
QPushButton#saveButton, QPushButton#saveTruckButton, QPushButton#saveProfileButton,
QPushButton#applyThemeButton, QPushButton#uploadReceiptButton, QPushButton#assignTemplateButton,
QPushButton#submitButton {
    background-color: #2563EB;
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
    color: #111827;
    border: 1px solid #CBD5E1;
}

QTableWidget {
    background-color: transparent;
    border: none;
    color: #111827;
    gridline-color: transparent;
    outline: none;
}

QTableWidget::item {
    background-color: transparent;
    border-bottom: 1px solid #E2E8F0;
    padding-left: 12px;
}

QTableWidget::item:selected {
    background-color: #DBEAFE;
}

QHeaderView::section {
    background-color: #F1F5F9;
    color: #64748B;
    border: none;
    border-bottom: 1px solid #CBD5E1;
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
    border: 1px solid #CBD5E1;
    color: #334155;
    border-radius: 6px;
    padding: 4px 7px;
}

QPushButton#viewButton:hover,
QPushButton#editButton:hover {
    background-color: #2563EB;
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
    background-color: #050B14;
    color: #FFFFFF;
}

QWidget#pageCard, QWidget#card, QWidget#tableCard, QWidget#toolbarCard, QWidget#formCard, QWidget#summaryCard,
QFrame#pageCard, QFrame#card, QFrame#tableCard, QFrame#toolbarCard, QFrame#formCard, QFrame#summaryCard {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 12px;
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
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

QPushButton#deleteButton, QPushButton#logoutButton {
    background-color: #DC2626;
    color: #FFFFFF;
    border: none;
}

QPushButton#cancelButton {
    background-color: #0B1626;
    color: #FFFFFF;
    border: 1px solid #263A57;
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
    border-bottom: 1px solid #132238;
    padding-left: 12px;
}

QTableWidget::item:selected {
    background-color: #132238;
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
    background-color: #172033;
    border: 1px solid #263A57;
    color: #CBD5E1;
    border-radius: 6px;
    padding: 4px 7px;
}

QPushButton#viewButton:hover,
QPushButton#editButton:hover {
    background-color: #1D4ED8;
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
QDialog, QWidget {
    background-color: #F4F7FB;
    color: #111827;
}

QFrame, QWidget#dialogCard, QWidget#formCard, QWidget#card {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 12px;
}

QLabel { color: #111827; }

QPushButton#saveButton, QPushButton#submitButton, QPushButton#addButton {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

QPushButton#cancelButton {
    background-color: #FFFFFF;
    color: #111827;
    border: 1px solid #CBD5E1;
}
)";
    }

    return R"(
QDialog, QWidget {
    background-color: #050B14;
    color: #FFFFFF;
}

QFrame, QWidget#dialogCard, QWidget#formCard, QWidget#card {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 12px;
}

QLabel { color: #FFFFFF; }

QPushButton#saveButton, QPushButton#submitButton, QPushButton#addButton {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

QPushButton#cancelButton {
    background-color: #0B1626;
    color: #FFFFFF;
    border: 1px solid #263A57;
}
)";
}

QString Theme::truckPageStyle(AppTheme theme)
{
    if (theme == AppTheme::Light) {
        return R"(
QWidget {
    background-color: #F4F7FB;
    color: #111827;
}

QWidget#pageCard, QWidget#card, QWidget#tableCard, QWidget#toolbarCard, QWidget#formCard, QWidget#summaryCard,
QFrame#pageCard, QFrame#card, QFrame#tableCard, QFrame#toolbarCard, QFrame#formCard, QFrame#summaryCard {
    background-color: #FFFFFF;
    border: 1px solid #CBD5E1;
    border-radius: 12px;
}

QLabel { color: #111827; }

QPushButton#addButton, QPushButton#newTemplateButton, QPushButton#addTruckButton,
QPushButton#assignTemplateButton, QPushButton#uploadReceiptButton, QPushButton#saveButton,
QPushButton#submitButton, QPushButton#useButton {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

QPushButton#deleteButton {
    background-color: #DC2626;
    color: #FFFFFF;
    border: none;
}
)";
    }

    return R"(
QWidget {
    background-color: #050B14;
    color: #FFFFFF;
}

QWidget#pageCard, QWidget#card, QWidget#tableCard, QWidget#toolbarCard, QWidget#formCard, QWidget#summaryCard,
QFrame#pageCard, QFrame#card, QFrame#tableCard, QFrame#toolbarCard, QFrame#formCard, QFrame#summaryCard {
    background-color: #0B1626;
    border: 1px solid #263A57;
    border-radius: 12px;
}

QLabel { color: #FFFFFF; }

QPushButton#addButton, QPushButton#newTemplateButton, QPushButton#addTruckButton,
QPushButton#assignTemplateButton, QPushButton#uploadReceiptButton, QPushButton#saveButton,
QPushButton#submitButton, QPushButton#useButton {
    background-color: #2563EB;
    color: #FFFFFF;
    border: none;
}

QPushButton#deleteButton {
    background-color: #DC2626;
    color: #FFFFFF;
    border: none;
}
)";
}
