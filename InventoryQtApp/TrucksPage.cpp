#include "TrucksPage.h"

TrucksPage::TrucksPage(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setupConnections();
    loadTrucks();
}

TrucksPage::~TrucksPage()
{
}

void TrucksPage::setupConnections()
{
    connect(ui.addTruckButton, &QPushButton::clicked, this, &TrucksPage::onAddTruckClicked);
    connect(ui.searchInput, &QLineEdit::textChanged, this, &TrucksPage::onSearchChanged);
}

void TrucksPage::loadTrucks()
{
    // Load trucks from backend
}

void TrucksPage::refreshTrucksList()
{
    loadTrucks();
}

void TrucksPage::onAddTruckClicked()
{
    // Open add truck dialog
}

void TrucksPage::onSearchChanged(const QString& text)
{
    // Filter trucks based on search text
}

void TrucksPage::onPageChanged(int page)
{
    // Load trucks for specified page
}
