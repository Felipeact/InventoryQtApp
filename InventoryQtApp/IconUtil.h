#pragma once

// Header-only line-icon helper shared by the table pages so per-row action
// buttons (view / edit / delete / reset-password) render crisp, themed SVG
// glyphs instead of emoji. Icons are recoloured for the active light/dark theme
// (read from QSettings) using a semantic palette: view/reset = navy, edit =
// amber, delete = red. Header-only so it needs no build-system entry.

#include <QColor>
#include <QIcon>
#include <QList>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QRectF>
#include <QSettings>
#include <QSize>
#include <QString>
#include <QSvgRenderer>
#include <QWidget>

namespace IconUtil {

inline QString iconBody(const QString& key)
{
    if (key == "eye")
        return QStringLiteral("<path d='M2 12s3.5-7 10-7 10 7 10 7-3.5 7-10 7-10-7-10-7z'/><circle cx='12' cy='12' r='3'/>");
    if (key == "pencil")
        return QStringLiteral("<path d='M12 20h9'/><path d='M16.5 3.5a2.12 2.12 0 0 1 3 3L7 19l-4 1 1-4z'/>");
    if (key == "trash")
        return QStringLiteral("<polyline points='3 6 5 6 21 6'/><path d='M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2'/><line x1='10' y1='11' x2='10' y2='17'/><line x1='14' y1='11' x2='14' y2='17'/>");
    if (key == "key")
        return QStringLiteral("<circle cx='7.5' cy='15.5' r='5.5'/><path d='m21 2-9.6 9.6'/><path d='m15.5 7.5 3 3L22 7l-3-3z'/>");
    return QString();
}

inline QPixmap renderIcon(const QString& body, const QColor& color, int side)
{
    const QString svg = QStringLiteral(
        "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='none' "
        "stroke='%1' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'>%2</svg>"
    ).arg(color.name(), body);

    QSvgRenderer renderer(svg.toUtf8());
    const qreal dpr = 2.0;
    QPixmap pm(int(side * dpr), int(side * dpr));
    pm.setDevicePixelRatio(dpr);
    pm.fill(Qt::transparent);

    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    // Render into an explicit logical rect so the 24x24 viewBox is scaled to
    // fit `side` units exactly (without a rect, render() uses the device-pixel
    // viewport as logical and draws the icon oversized/clipped).
    renderer.render(&painter, QRectF(0, 0, side, side));
    painter.end();
    return pm;
}

inline QIcon lineIcon(const QString& key, const QColor& color, int side = 16)
{
    return QIcon(renderIcon(iconBody(key), color, side));
}

inline bool isDarkTheme()
{
    QSettings settings("InventorySystem", "InventoryQtApp");
    return settings.value("appearance/theme", "Light").toString() == "Dark";
}

// Replace the emoji/text of a recognised action button with a themed line icon.
inline void iconifyActionButton(QPushButton* button)
{
    if (!button) {
        return;
    }

    const QString name = button->objectName();
    const bool dark = isDarkTheme();

    QString key;
    QString tip;
    QColor color;

    if (name == "viewButton") {
        key = "eye"; tip = "View";
        color = dark ? QColor("#7FAAD6") : QColor("#1D4E89");
    } else if (name == "editButton") {
        key = "pencil"; tip = "Edit";
        color = dark ? QColor("#FBBF24") : QColor("#D97706");
    } else if (name == "deleteButton") {
        key = "trash"; tip = "Delete";
        color = dark ? QColor("#F2566A") : QColor("#D7263D");
    } else if (name == "resetButton") {
        key = "key"; tip = "Reset password";
        color = dark ? QColor("#7FAAD6") : QColor("#1D4E89");
    } else {
        return;
    }

    button->setText(QString());
    button->setIcon(lineIcon(key, color, 16));
    button->setIconSize(QSize(16, 16));
    if (button->toolTip().isEmpty()) {
        button->setToolTip(tip);
    }
}

// Iconify every recognised action button found under a container (a row's
// action cell, or an entire table — findChildren recurses into cell widgets).
inline void iconifyActions(QWidget* container)
{
    if (!container) {
        return;
    }
    const QList<QPushButton*> buttons = container->findChildren<QPushButton*>();
    for (QPushButton* button : buttons) {
        iconifyActionButton(button);
    }
}

} // namespace IconUtil
