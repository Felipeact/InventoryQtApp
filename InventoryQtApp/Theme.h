#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>

class Theme
{
public:
    static void applyApplication();
    static void applyLogin(QMainWindow* window);
    static void applyShell(QMainWindow* window);
    static void applySidebar(QWidget* sidebar);
    static void applyTopbar(QWidget* topbar);
    static void applyDashboard(QWidget* page);
    static void applyDataPage(QWidget* page);
    static void applyDialog(QDialog* dialog);
    static void styleTable(QTableWidget* table);
    static void styleActionButton(QPushButton* button, const QString& type);
    static QString globalStyleSheet();
};
