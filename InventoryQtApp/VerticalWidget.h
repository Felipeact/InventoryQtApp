// VerticalWidget.h - Top bar displaying user information
#pragma once

#include <QWidget>
#include <QString>
#include <QTimer>
#include <QRect>
#include <string>

#include "ui_VerticalWidget.h"
#include "Theme.h"

class QLabel;

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

    /** Global-screen rect just below the search field, to anchor the results dropdown. */
    QRect searchFieldGlobalRect() const;

    /** Show a small badge with the number of pending notifications (0 hides it). */
    void setNotificationCount(int count);

signals:
    void globalSearchTextChanged(const QString& text);
    void notificationRequested();

    // Keyboard control forwarded from the search box to the results dropdown.
    void searchNavigated(int delta); // +1 = down, -1 = up
    void searchActivated();          // Enter
    void searchDismissed();          // Escape / cancel

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    Ui::VerticalWidget ui;

    std::string role;
    std::string username;

    QTimer* searchDebounceTimer = nullptr;
    QString pendingSearchText;

    QLabel* notificationBadge = nullptr;
};
