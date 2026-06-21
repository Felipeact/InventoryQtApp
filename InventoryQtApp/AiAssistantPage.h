#pragma once

#include <QWidget>
#include <QString>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

#include "AiService.h"
#include "Theme.h"

class QTextBrowser;
class QLineEdit;
class QPushButton;
class QLabel;

// In-app AI assistant chat page. Mirrors the web assistant: ask questions, build
// reports, and take actions in plain English. Network calls run on a worker
// thread so the UI stays responsive while the model thinks (which can take a few
// seconds).
class AiAssistantPage : public QWidget
{
    Q_OBJECT

public:
    explicit AiAssistantPage(AiService* aiService, QWidget* parent = nullptr);

    void applyTheme(Theme::AppTheme theme);

private:
    AiService* aiService = nullptr;

    QLabel* titleLabel = nullptr;
    QLabel* subtitleLabel = nullptr;
    QTextBrowser* transcript = nullptr;
    QLineEdit* input = nullptr;
    QPushButton* sendButton = nullptr;

    // Full conversation as a JSON array of { role, content } turns, resent each
    // request so the assistant has context.
    nlohmann::json history = nlohmann::json::array();
    bool busy = false;

    void sendMessage();
    void appendBubble(const QString& who, const QString& text, bool isUser);
    void appendActions(const std::vector<std::string>& tools);
    void setBusy(bool value);

    static QString escapeHtml(const QString& text);
};
