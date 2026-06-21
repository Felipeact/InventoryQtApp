#include "AiAssistantPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QThread>
#include <QPointer>
#include <QMetaObject>
#include <QFont>

AiAssistantPage::AiAssistantPage(AiService* aiService, QWidget* parent)
    : QWidget(parent),
    aiService(aiService)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(14);

    titleLabel = new QLabel("AI Assistant", this);
    titleLabel->setObjectName("aiTitle");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(titleFont.pointSize() + 6);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    subtitleLabel = new QLabel(
        "Ask questions, build reports, and take actions across your inventory — in plain English.",
        this
    );
    subtitleLabel->setObjectName("aiSubtitle");
    subtitleLabel->setWordWrap(true);

    transcript = new QTextBrowser(this);
    transcript->setObjectName("aiTranscript");
    transcript->setOpenExternalLinks(false);
    transcript->setReadOnly(true);

    input = new QLineEdit(this);
    input->setObjectName("aiInput");
    input->setPlaceholderText("Ask the assistant…  (e.g. \"how is my stock?\")");
    input->setMinimumHeight(40);

    sendButton = new QPushButton("Send", this);
    sendButton->setObjectName("aiSend");
    sendButton->setMinimumHeight(40);
    sendButton->setCursor(Qt::PointingHandCursor);

    auto* inputRow = new QHBoxLayout();
    inputRow->setSpacing(10);
    inputRow->addWidget(input, 1);
    inputRow->addWidget(sendButton, 0);

    root->addWidget(titleLabel);
    root->addWidget(subtitleLabel);
    root->addWidget(transcript, 1);
    root->addLayout(inputRow);

    connect(sendButton, &QPushButton::clicked, this, &AiAssistantPage::sendMessage);
    connect(input, &QLineEdit::returnPressed, this, &AiAssistantPage::sendMessage);

    // Friendly opener (not part of the conversation history sent to the model).
    appendBubble(
        "Assistant",
        "Hi! I can read your stock, generate reports, and take actions like adding "
        "products or adjusting quantities — all within your permissions. What do you need?",
        false
    );
}

void AiAssistantPage::setBusy(bool value)
{
    busy = value;
    input->setEnabled(!value);
    sendButton->setEnabled(!value);
    sendButton->setText(value ? "…" : "Send");
}

QString AiAssistantPage::escapeHtml(const QString& text)
{
    QString out = text;
    out.replace('&', "&amp;");
    out.replace('<', "&lt;");
    out.replace('>', "&gt;");
    out.replace('\n', "<br>");
    return out;
}

void AiAssistantPage::appendBubble(const QString& who, const QString& text, bool isUser)
{
    const QString labelColor = isUser ? "#2563EB" : "#0F766E";
    const QString html = QStringLiteral(
        "<p style='margin:6px 0;'>"
        "<b style='color:%1;'>%2</b><br>"
        "<span>%3</span></p>"
    ).arg(labelColor, who.toHtmlEscaped(), escapeHtml(text));

    transcript->append(html);
}

void AiAssistantPage::appendActions(const std::vector<std::string>& tools)
{
    if (tools.empty()) {
        return;
    }

    QStringList names;
    for (const std::string& tool : tools) {
        names << QString::fromStdString(tool);
    }

    const QString html = QStringLiteral(
        "<p style='margin:0 0 10px 0;color:#94A3B8;font-size:11px;'>↳ ran: %1</p>"
    ).arg(names.join(", ").toHtmlEscaped());

    transcript->append(html);
}

void AiAssistantPage::sendMessage()
{
    if (busy || !aiService) {
        return;
    }

    const QString text = input->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    appendBubble("You", text, true);
    history.push_back({ { "role", "user" }, { "content", text.toStdString() } });

    input->clear();
    setBusy(true);

    // Snapshot the conversation for the worker thread.
    const nlohmann::json snapshot = history;
    AiService* service = aiService;
    QPointer<AiAssistantPage> self = this;

    QThread* worker = QThread::create([self, service, snapshot]() {
        nlohmann::json result = service->chat(snapshot);

        if (!self) {
            return;
        }

        QMetaObject::invokeMethod(
            self,
            [self, result]() {
                if (!self) {
                    return;
                }

                self->setBusy(false);

                if (result.contains("error")) {
                    self->appendBubble(
                        "Assistant",
                        QString::fromStdString(result.value("error", std::string("Something went wrong."))),
                        false
                    );
                    return;
                }

                const std::string reply = result.value("reply", std::string());
                self->history.push_back({ { "role", "assistant" }, { "content", reply } });
                self->appendBubble("Assistant", QString::fromStdString(reply), false);

                if (result.contains("actions") && result["actions"].is_array()) {
                    std::vector<std::string> tools;
                    for (const auto& action : result["actions"]) {
                        if (action.contains("tool") && action["tool"].is_string()) {
                            tools.push_back(action["tool"].get<std::string>());
                        }
                    }
                    self->appendActions(tools);
                }
            },
            Qt::QueuedConnection
        );
    });

    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void AiAssistantPage::applyTheme(Theme::AppTheme theme)
{
    const bool dark = (theme == Theme::AppTheme::Dark);

    const QString pageBg = dark ? "#0F172A" : "#F7F9FC";
    const QString cardBg = dark ? "#1E293B" : "#FFFFFF";
    const QString textColor = dark ? "#E2E8F0" : "#1E293B";
    const QString subColor = dark ? "#94A3B8" : "#64748B";
    const QString border = dark ? "#334155" : "#E2E8F0";
    const QString accent = "#F59E0B"; // amber accent, matching the app

    setStyleSheet(QStringLiteral(
        "AiAssistantPage { background:%1; }"
        "QLabel#aiTitle { color:%3; }"
        "QLabel#aiSubtitle { color:%4; }"
        "QTextBrowser#aiTranscript { background:%2; color:%3; border:1px solid %5;"
        "  border-radius:12px; padding:10px; }"
        "QLineEdit#aiInput { background:%2; color:%3; border:1px solid %5;"
        "  border-radius:10px; padding:0 12px; }"
        "QPushButton#aiSend { background:%6; color:#1F2937; border:none;"
        "  border-radius:10px; padding:0 18px; font-weight:600; }"
        "QPushButton#aiSend:disabled { background:%5; color:%4; }"
    ).arg(pageBg, cardBg, textColor, subColor, border, accent));
}
