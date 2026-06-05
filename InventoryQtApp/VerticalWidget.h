// VerticalWidget.h - Top bar displaying user information
#pragma once

#include <QWidget>
#include <QString>
#include <string>

#include "ui_VerticalWidget.h"
#include "Theme.h"

class VerticalWidget : public QWidget
{
    Q_OBJECT

public:
    VerticalWidget(
        const std::string& userRole,
        const std::string& userName,
        QWidget* parent = nullptr
    );

    ~VerticalWidget();

    void applyTheme(Theme::AppTheme theme);

    void setUserInfo(
        const std::string& role,
        const std::string& userName
    );

    void clearSearch();
    void focusSearch();

signals:
    void globalSearchTextChanged(const QString& text);
    void notificationRequested();

private:
    Ui::VerticalWidget ui;

    std::string role;
    std::string username;
};