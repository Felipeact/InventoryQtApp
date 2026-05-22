#include "MyTruckStockPage.h"

#include <QComboBox>
#include <QLineEdit>

MyTruckStockPage::MyTruckStockPage(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setupConnections();
    loadStock();
}

MyTruckStockPage::~MyTruckStockPage()
{
}

void MyTruckStockPage::setupConnections()
{
    connect(ui.searchInput, &QLineEdit::textChanged,
        this, &MyTruckStockPage::onSearchChanged);
}

void MyTruckStockPage::loadTrucks()
{
    // Not used anymore because the new UI has no truckSelector.
}

void MyTruckStockPage::loadStock()
{
    // Load stock items for current technician.
}

void MyTruckStockPage::refreshStock()
{
    loadStock();
}

void MyTruckStockPage::onTruckSelected(int index)
{
    Q_UNUSED(index);
    loadStock();
}

void MyTruckStockPage::onSearchChanged(const QString& text)
{
    Q_UNUSED(text);
    // Filter stock items later.
}

void MyTruckStockPage::onUseItemClicked()
{
    // Open UseTruckItemDialog later.
}