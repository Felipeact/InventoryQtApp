// VerticalWidget.cpp - Implementation of the user information bar
#include "VerticalWidget.h"
#include "Theme.h"

#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QLabel>
#include <QEvent>
#include <QKeyEvent>

VerticalWidget::VerticalWidget(
    const std::string& userRole,
    const std::string& userName,
    QWidget* parent
)
    : QWidget(parent),
    role(userRole),
    username(userName)
{
    ui.setupUi(this);

    setUserInfo(userRole, userName);

    searchDebounceTimer = new QTimer(this);
    searchDebounceTimer->setSingleShot(true);
    searchDebounceTimer->setInterval(400);

    connect(
        searchDebounceTimer,
        &QTimer::timeout,
        this,
        [this]() {
            emit globalSearchTextChanged(pendingSearchText);
        }
    );

    connect(
        ui.searchInput,
        &QLineEdit::textChanged,
        this,
        [this](const QString& text) {
            pendingSearchText = text;
            searchDebounceTimer->start();
        }
    );

    connect(
        ui.notificationButton,
        &QPushButton::clicked,
        this,
        [this]() {
            emit notificationRequested();
        }
    );

    // Let arrow keys / Enter / Escape in the search box drive the results dropdown
    // without the field ever losing focus.
    ui.searchInput->installEventFilter(this);

    // Small unread-count badge pinned to the top-right of the notification bell.
    notificationBadge = new QLabel(ui.notificationButton);
    notificationBadge->setObjectName("notificationBadge");
    notificationBadge->setAlignment(Qt::AlignCenter);
    notificationBadge->setAttribute(Qt::WA_TransparentForMouseEvents);
    notificationBadge->setStyleSheet(
        "background:#ef4444;color:#ffffff;border-radius:8px;"
        "font-size:10px;font-weight:bold;padding:0 4px;"
    );
    notificationBadge->hide();
}

bool VerticalWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui.searchInput && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
        case Qt::Key_Down:
            emit searchNavigated(1);
            return true;
        case Qt::Key_Up:
            emit searchNavigated(-1);
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            emit searchActivated();
            return true;
        case Qt::Key_Escape:
            ui.searchInput->clear();
            emit searchDismissed();
            return true;
        default:
            break;
        }
    }

    return QWidget::eventFilter(watched, event);
}

QRect VerticalWidget::searchFieldGlobalRect() const
{
    const QPoint topLeft =
        ui.searchInput->mapToGlobal(QPoint(0, ui.searchInput->height()));
    return QRect(topLeft, QSize(ui.searchInput->width(), ui.searchInput->height()));
}

void VerticalWidget::setNotificationCount(int count)
{
    if (!notificationBadge) {
        return;
    }

    if (count <= 0) {
        notificationBadge->hide();
        return;
    }

    notificationBadge->setText(count > 99 ? QStringLiteral("99+") : QString::number(count));
    notificationBadge->adjustSize();

    const int height = 16;
    const int width = qMax(height, notificationBadge->width());
    notificationBadge->setFixedSize(width, height);

    // Pin to the top-right corner of the bell button.
    notificationBadge->move(
        qMax(0, ui.notificationButton->width() - width + 4),
        -2
    );
    notificationBadge->raise();
    notificationBadge->show();
}

VerticalWidget::~VerticalWidget()
{
}

void VerticalWidget::setUserInfo(
    const std::string& role,
    const std::string& userName
)
{
    this->role = role;
    this->username = userName;

    ui.welcomeLabel->setText(
        "Hello " + QString::fromStdString(userName)
    );

    if (!userName.empty()) {
        ui.avatarLabel->setText(
            QString::fromStdString(userName.substr(0, 1)).toUpper()
        );
    }
}

void VerticalWidget::clearSearch()
{
    ui.searchInput->clear();
}

void VerticalWidget::focusSearch()
{
    ui.searchInput->setFocus();
    ui.searchInput->selectAll();
}

void VerticalWidget::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::verticalBarStyle(theme)
    );
}