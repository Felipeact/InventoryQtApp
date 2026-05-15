#include "Theme.h"
#include <QApplication>
#include <QScrollBar>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QTableWidget>
#include <QFrame>

QString Theme::globalStyleSheet()
{
    return R"(
* {
    font-family: 'Segoe UI', 'Inter', Arial;
    font-size: 13px;
}
QMainWindow, QWidget#centralWidget,
QWidget#DashboardPageClass, QWidget#ItemsPageClass, QWidget#AssetsPageClass, QWidget#ScanPageClass {
    background: #07111f;
    color: #e6eefb;
}
QLabel { color: #e6eefb; background: transparent; }
QLineEdit, QSpinBox, QComboBox, QPlainTextEdit {
    background: #0b1626;
    border: 1px solid #243751;
    border-radius: 8px;
    color: #e6eefb;
    padding: 9px 13px;
    min-height: 30px;
    selection-background-color: #2563eb;
}
QLineEdit:focus, QSpinBox:focus, QComboBox:focus, QPlainTextEdit:focus {
    border: 1px solid #2f80ed;
    background: #0e1b2f;
}
QLineEdit::placeholder { color: #8191a8; }
QPushButton {
    background: #0e1b2d;
    border: 1px solid #243751;
    border-radius: 8px;
    color: #e6eefb;
    padding: 8px 14px;
    font-weight: 650;
}
QPushButton:hover { background: #14253d; border-color: #2f80ed; color: white; }
QPushButton:pressed { background: #0b1525; }
QPushButton#addItemButton, QPushButton#addAssetButton, QPushButton#submitButton,
QPushButton#saveButton, QPushButton#loginButton {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #2563eb, stop:1 #1d4ed8);
    border: none;
    color: white;
    border-radius: 9px;
    padding: 10px 18px;
    font-weight: 750;
}
QPushButton#addItemButton:hover, QPushButton#addAssetButton:hover, QPushButton#submitButton:hover,
QPushButton#saveButton:hover, QPushButton#loginButton:hover {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #3b82f6, stop:1 #2563eb);
}
QPushButton#filterItemButton, QPushButton#filterAssetButton, QPushButton#cancelButton {
    background: #0b1626;
    border: 1px solid #263a55;
}
QScrollBar:vertical { background: transparent; width: 8px; margin: 6px 2px 6px 2px; }
QScrollBar::handle:vertical { background: rgba(92, 116, 148, 0.70); border-radius: 4px; min-height: 36px; }
QScrollBar::handle:vertical:hover { background: #6d86aa; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }
QScrollBar:horizontal { background: transparent; height: 8px; }
QScrollBar::handle:horizontal { background: rgba(92, 116, 148, 0.70); border-radius: 4px; min-width: 36px; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }
QToolTip { background: #0b1626; color: white; border: 1px solid #2f80ed; }
)";
}

void Theme::applyApplication()
{
    qApp->setStyleSheet(globalStyleSheet());
}

void Theme::applyLogin(QMainWindow* window)
{
    window->setFixedSize(360, 520);
    window->setStyleSheet(globalStyleSheet() + R"(
QMainWindow {
    background: #050816;
}
QWidget#centralWidget {
    background: qradialgradient(cx:0.50, cy:0.28, radius:0.90, fx:0.50, fy:0.25,
        stop:0 #13243a,
        stop:0.35 #0b1828,
        stop:1 #050816);
    border: none;
    border-radius: 0px;
}
QFrame#loginCard {
    background: transparent;
    border: none;
    border-radius: 0px;
}
QLabel#logoLabel {
    color: #60a5fa;
    font-size: 22px;
    min-width: 22px;
}
QLabel#titleLabel {
    color: #ffffff;
    font-size: 20px;
    font-weight: 850;
    background: transparent;
}
QLabel#subtitleLabel {
    color: #93a4bb;
    font-size: 12px;
    background: transparent;
}
QLabel#emailLabel,
QLabel#passwordLabel {
    color: #cbd5e1;
    font-size: 11px;
    font-weight: 600;
    background: transparent;
}
QLineEdit#emailInput,
QLineEdit#passwordInput {
    background: rgba(8, 17, 31, 0.95);
    border: 1px solid rgba(74, 109, 163, 0.55);
    border-radius: 7px;
    color: #ffffff;
    padding: 0 10px;
    font-size: 12px;
    min-height: 34px;
}
QLineEdit#emailInput:focus,
QLineEdit#passwordInput:focus {
    border: 1px solid #3b82f6;
    background: rgba(11, 26, 46, 0.98);
}
QLineEdit::placeholder {
    color: #7f8da3;
}
QCheckBox#rememberCheck {
    color: #cbd5e1;
    font-size: 11px;
    spacing: 6px;
    background: transparent;
}
QCheckBox#rememberCheck::indicator {
    width: 12px;
    height: 12px;
    border-radius: 3px;
    border: 1px solid #3b82f6;
    background: #0b1626;
}
QCheckBox#rememberCheck::indicator:checked {
    background: #2563eb;
    border: 1px solid #2563eb;
}
QPushButton#forgotPasswordButton {
    color: #60a5fa;
    background: transparent;
    border: none;
    padding: 0;
    font-size: 11px;
    font-weight: 500;
}
QPushButton#forgotPasswordButton:hover {
    color: #93c5fd;
    text-decoration: underline;
}
QPushButton#loginButton {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #2563eb,
        stop:1 #1d4ed8);
    border: none;
    border-radius: 7px;
    color: white;
    font-size: 12px;
    font-weight: 750;
    min-height: 38px;
}
QPushButton#loginButton:hover {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #3b82f6,
        stop:1 #2563eb);
}
QLabel#statusLabel {
    color: #facc15;
    font-size: 11px;
    font-weight: 700;
    min-height: 16px;
    background: transparent;
}
QLabel#footerLabel {
    color: #64748b;
    font-size: 10px;
    background: transparent;
}
)");
}

void Theme::applyShell(QMainWindow* window)
{
    window->setStyleSheet(globalStyleSheet() + R"(
QWidget#centralWidget { background: #07111f; }
QWidget#sidebarContainer {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #0b1727, stop:1 #07111f);
    border: 1px solid #243751;
    border-radius: 12px;
}
QWidget#mainAreaContainer { background: transparent; }
QWidget#verticalContainer {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #0b1626, stop:1 #101827);
    border: 1px solid #243751;
    border-radius: 10px;
}
QStackedWidget#mainStack { background: transparent; border: none; }
)");
}

void Theme::applySidebar(QWidget* sidebar)
{
    sidebar->setFixedWidth(184);
    sidebar->setStyleSheet(R"(
QWidget#SidebarWidgetClass { background: transparent; }
QLabel#logoLabel {
    color: #ffffff;
    font-size: 17px;
    font-weight: 850;
    padding: 16px 0 22px 4px;
}
QPushButton {
    background: transparent;
    color: #dbeafe;
    border: none;
    border-radius: 8px;
    text-align: left;
    padding: 10px 14px;
    font-weight: 650;
    min-height: 18px;
}
QPushButton:hover { background: rgba(37, 99, 235, 0.18); color: #ffffff; }
QPushButton:checked {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #1d4ed8, stop:1 #1e3a8a);
    color: #ffffff;
}
QPushButton#logoutButton {
    color: #fb7185;
    background: rgba(127, 29, 29, 0.25);
    margin-top: 10px;
}
QPushButton#logoutButton:hover { background: rgba(220, 38, 38, 0.32); color: white; }
)");
}

void Theme::applyTopbar(QWidget* topbar)
{
    topbar->setStyleSheet(R"(
QWidget#VerticalWidgetClass { background: transparent; }
QLabel#usernameLabel { color: white; font-size: 15px; font-weight: 850; }
QLabel#roleLabel { color: #9fb0c7; font-size: 12px; text-transform: uppercase; }
QWidget#searchContainer {
    background: #0b1626;
    border: 1px solid #243751;
    border-radius: 8px;
}
QLineEdit#searchInputButton {
    border: none;
    background: transparent;
    color: #e5eefb;
    padding: 0 10px;
    min-height: 30px;
}
QPushButton#searchButton {
    border: none;
    background: transparent;
    color: white;
    padding: 0 12px;
    min-width: 58px;
}
)");
}

void Theme::applyDashboard(QWidget* page)
{
    page->setStyleSheet(globalStyleSheet() + R"(
QScrollArea { border: none; background: transparent; }
QWidget#scrollAreaWidgetContents_2 { background: transparent; }
QWidget#itemListCard, QWidget#assetListCard_3, QWidget#itemSummaryCard, QWidget#assetCard, QWidget#itemTotalCard {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #0d1828, stop:1 #091322);
    border: 1px solid #263a57;
    border-radius: 12px;
}
QLabel#itemListTitleLabel, QLabel#assetListTitleLabel, QLabel#itemSummaryTitle, QLabel#assetSummaryTitle, QLabel#itemTotalTitle {
    font-size: 17px;
    font-weight: 850;
    color: #ffffff;
    background: transparent;
}
QPushButton#viewAllItemsButton, QPushButton#viewAllIAssetButton {
    color: #60a5fa;
    background: transparent;
    border: none;
    font-weight: 700;
}
QLabel#leftIconItem, QLabel#rightIconItem, QLabel#leftIconAsset, QLabel#rightIconAsset {
    color: #60a5fa;
    font-size: 26px;
}
QLabel#leftTotalItem, QLabel#rightTotalItem, QLabel#leftTotalAsset, QLabel#rightTotalAsset {
    color: #ffffff;
    font-size: 26px;
    font-weight: 850;
}
QLabel#leftDescriptionItem, QLabel#rightDescriptionItem, QLabel#leftDescriptionAsset, QLabel#rightDescriptionAsset {
    color: #cbd5e1;
    font-size: 12px;
    font-weight: 700;
}
QFrame#line, QFrame#frame, QFrame#frame_2, QFrame#frame_3, QFrame#frame_4 { color: #3b4c63; background: #3b4c63; }
)");
}

void Theme::applyDataPage(QWidget* page)
{
    page->setStyleSheet(globalStyleSheet() + R"(

/* =========================
   TOOLBAR
========================= */

QWidget#toolbarContainer {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #0b1626,
        stop:1 #101827);
    border: 1px solid #263a57;
    border-radius: 12px;
}

QWidget#searchContainer {
    background: #0f172a;
    border: 1px solid #263a57;
    border-radius: 8px;
}

QLineEdit#itemSearchInput,
QLineEdit#assetSearchInput {
    border: none;
    background: transparent;
    min-height: 32px;
    padding-left: 12px;
    padding-right: 4px;
}

QPushButton#itemSearchButton,
QPushButton#assetSearchButton {
    background: transparent;
    border: none;
    padding: 0 10px;
    min-width: 58px;
    color: white;
}

QPushButton#itemSearchButton:hover,
QPushButton#assetSearchButton:hover {
    background: transparent;
    color: #93c5fd;
}

/* =========================
   TABLE CARD
========================= */

QWidget#tableCard {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
        stop:0 #0d1828,
        stop:1 #091322);

    border: 1px solid #263a57;
    border-radius: 12px;
}

/* =========================
   PAGINATION
========================= */

QPushButton#prevPageButton,
QPushButton#page2Button,
QPushButton#nextPageButton {
    background-color: #111C2D;
    border: 1px solid #22324A;
    border-radius: 8px;
    color: white;
    min-width: 32px;
    min-height: 32px;
    font-size: 12px;
    font-weight: 600;
}

QPushButton#prevPageButton:hover,
QPushButton#page2Button:hover,
QPushButton#nextPageButton:hover {
    background-color: #1A2940;
    border: 1px solid #2D4B73;
}

QPushButton#activePageButton {
    background-color: #2563EB;
    border: none;
    border-radius: 8px;
    color: white;
    min-width: 32px;
    min-height: 32px;
    font-size: 12px;
    font-weight: 700;
}

QPushButton#activePageButton:hover {
    background-color: #3B82F6;
}

QComboBox#pageSizeCombo {
    background-color: #111C2D;
    border: 1px solid #22324A;
    border-radius: 8px;
    color: white;
    padding: 6px 10px;
    min-height: 32px;
}

QComboBox#pageSizeCombo::drop-down {
    border: none;
}

/* =========================
   ACTION BUTTONS
========================= */

QPushButton#viewButton,
QPushButton#editButton,
QPushButton#deleteButton {
    min-width: 28px;
    max-width: 28px;

    min-height: 28px;
    max-height: 28px;

    border-radius: 7px;

    background-color: #0E1A2B;
    border: 1px solid #1E3A5F;

    color: white;
    font-size: 12px;
    font-weight: 700;
    padding: 0;
}

QPushButton#viewButton:hover {
    background-color: #1D4ED8;
    border: 1px solid #2563EB;
}

QPushButton#editButton:hover {
    background-color: #2563EB;
    border: 1px solid #3B82F6;
}

QPushButton#deleteButton:hover {
    background-color: #DC2626;
    border: 1px solid #EF4444;
}

)");
}

void Theme::applyDialog(QDialog* dialog)
{
    dialog->setStyleSheet(globalStyleSheet() + R"(
QDialog {
    background: #07111f;
    border: 1px solid #263a57;
}
QWidget#headerContainer {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #1d4ed8, stop:1 #0f2559);
    border-top-left-radius: 10px;
    border-top-right-radius: 10px;
    min-height: 48px;
}
QWidget#widget_2 { background: #07111f; }
QWidget#widget_3 { background: #07111f; }
QLabel#titleLabel { color: #ffffff; font-size: 18px; font-weight: 850; }
QPushButton#closeButton { background: transparent; border: none; font-size: 20px; color: white; }
QPushButton#closeButton:hover { background: rgba(255,255,255,0.08); }
QLabel { color: #e5eefb; }
QLineEdit, QSpinBox, QComboBox, QPlainTextEdit { background: #0b1626; border: 1px solid #263a57; }
)");
}

void Theme::styleTable(QTableWidget* table)
{
    table->setAlternatingRowColors(false);
    table->setShowGrid(false);
    table->setFrameShape(QFrame::NoFrame);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setHighlightSections(false);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    table->horizontalHeader()->setFixedHeight(44);
    table->verticalHeader()->setDefaultSectionSize(46);
    table->setStyleSheet(R"(
QTableWidget {
    background: #0b1626;
    color: #dbeafe;
    border: 1px solid #243751;
    border-radius: 10px;
    gridline-color: transparent;
    selection-background-color: #17345c;
    outline: none;
}
QHeaderView::section {
    background: #0f1b2d;
    color: #a8bdd8;
    border: none;
    border-bottom: 1px solid #243751;
    padding-left: 12px;
    font-weight: 750;
}
QTableWidget::item {
    border-bottom: 1px solid #142238;
    padding-left: 10px;
}
QTableWidget::item:hover { background: #111f35; }
QTableWidget::item:selected { background: #17345c; color: white; }
)");
}

void Theme::styleActionButton(QPushButton* button, const QString& type)
{
    button->setFixedSize(28, 28);
    QString color = type == "danger" ? "#dc2626" : (type == "edit" ? "#2563eb" : "#0f3b70");
    button->setStyleSheet(QString(R"(
QPushButton { background: %1; color: white; border: none; border-radius: 7px; font-weight: 800; padding: 0; }
QPushButton:hover { background: #3b82f6; }
)").arg(color));
}
