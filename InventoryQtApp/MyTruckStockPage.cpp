#include "MyTruckStockPage.h"

MyTruckStockPage::MyTruckStockPage(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setupConnections();
    loadTrucks();
    loadStock();
}

MyTruckStockPage::~MyTruckStockPage()
{
}

void MyTruckStockPage::setupConnections()
{
    connect(ui.truckSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), 
        this, &MyTruckStockPage::onTruckSelected);
    connect(ui.searchInput, &QLineEdit::textChanged, this, &MyTruckStockPage::onSearchChanged);
}

void MyTruckStockPage::loadTrucks()
{
    // Load assigned trucks for current technician
}

void MyTruckStockPage::loadStock()
{
    // Load stock items for selected truck
}

void MyTruckStockPage::refreshStock()
{
    loadStock();
}

void MyTruckStockPage::onTruckSelected(int index)
{
    loadStock();
}

void MyTruckStockPage::onSearchChanged(const QString& text)
{
    // Filter stock items based on search text
}

void MyTruckStockPage::onUseItemClicked()
{
    // Open use item dialog
}
