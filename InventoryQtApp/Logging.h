// Logging.h - Centralized, production-safe logging helpers.
//
// These route through Qt's logging framework instead of std::cout/std::cerr,
// so output can be filtered/redirected by a Qt message handler and never leaks
// to a raw stdout stream. Debug-level messages are compiled out entirely in
// release builds (QT_NO_DEBUG_OUTPUT is defined for the Release configuration),
// so endpoints and server payloads are not logged in production.
#pragma once

#include <string>

#include <QtGlobal>
#include <QDebug>
#include <QString>

namespace inv
{
    // Verbose, developer-facing messages. No-op in release builds.
    inline void logDebug(const std::string& message)
    {
        qDebug().noquote() << "[Inventory]" << QString::fromStdString(message);
    }

    // Non-fatal problems worth surfacing in diagnostics (kept in release).
    inline void logWarning(const std::string& message)
    {
        qWarning().noquote() << "[Inventory]" << QString::fromStdString(message);
    }
}
