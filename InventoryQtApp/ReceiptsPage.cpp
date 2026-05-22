#include "ReceiptsPage.h"

ReceiptsPage::ReceiptsPage(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setupConnections();
    loadReceipts();
}

ReceiptsPage::~ReceiptsPage()
{
}

void ReceiptsPage::setupConnections()
{
    connect(ui.uploadButton, &QPushButton::clicked, this, &ReceiptsPage::onUploadReceiptClicked);
    connect(ui.searchInput, &QLineEdit::textChanged, this, &ReceiptsPage::onSearchChanged);
}

void ReceiptsPage::loadReceipts()
{
    // Load receipts from backend
}

void ReceiptsPage::refreshReceipts()
{
    loadReceipts();
}

void ReceiptsPage::onUploadReceiptClicked()
{
    // Open upload receipt dialog
}

void ReceiptsPage::onSearchChanged(const QString& text)
{
    // Filter receipts based on search text
}

void ReceiptsPage::onPageChanged(int page)
{
    // Load receipts for specified page
}
