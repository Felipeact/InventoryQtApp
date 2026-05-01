#include "DashboardPage.h"

DashboardPage::DashboardPage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    setupItemListTable();
}

DashboardPage::~DashboardPage()
{}

void DashboardPage::setupItemListTable()
{
    ui.itemListTable->setColumnCount(4);
    ui.itemListTable->setHorizontalHeaderLabels(
        QStringList() << "Name" << "Image" << "Store" << "Amount"
    );

    ui.itemListTable->setRowCount(4);

    ui.itemListTable->setItem(0, 0, new QTableWidgetItem("Gas Kitting"));
    ui.itemListTable->setItem(0, 1, new QTableWidgetItem("Image"));
    ui.itemListTable->setItem(0, 2, new QTableWidgetItem("22 House Store"));
    ui.itemListTable->setItem(0, 3, new QTableWidgetItem("1 pcs"));

    ui.itemListTable->setItem(1, 0, new QTableWidgetItem("Condet"));
    ui.itemListTable->setItem(1, 1, new QTableWidgetItem("Image"));
    ui.itemListTable->setItem(1, 2, new QTableWidgetItem("HQ Main Store"));
    ui.itemListTable->setItem(1, 3, new QTableWidgetItem("3 pcs"));

    ui.itemListTable->setItem(2, 0, new QTableWidgetItem("Condet"));
    ui.itemListTable->setItem(2, 1, new QTableWidgetItem("Image"));
    ui.itemListTable->setItem(2, 2, new QTableWidgetItem("HQ Main Store"));
    ui.itemListTable->setItem(2, 3, new QTableWidgetItem("5 pcs"));

    ui.itemListTable->setItem(3, 0, new QTableWidgetItem("Condet"));
    ui.itemListTable->setItem(3, 1, new QTableWidgetItem("Image"));
    ui.itemListTable->setItem(3, 2, new QTableWidgetItem("HQ Main Store"));
    ui.itemListTable->setItem(3, 3, new QTableWidgetItem("5 pcs"));

    ui.itemListTable->verticalHeader()->setVisible(false);
    ui.itemListTable->horizontalHeader()->setVisible(true);
    ui.itemListTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui.itemListTable->horizontalHeader()->setFixedHeight(48);
    ui.itemListTable->horizontalHeader()->setHighlightSections(false);

    ui.itemListTable->setShowGrid(false);
    ui.itemListTable->setFrameShape(QFrame::NoFrame);
    ui.itemListTable->setFocusPolicy(Qt::NoFocus);
    ui.itemListTable->setSelectionMode(QAbstractItemView::NoSelection);

    ui.itemListTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui.itemListTable->setColumnWidth(0, 115);
    ui.itemListTable->setColumnWidth(1, 85);
    ui.itemListTable->setColumnWidth(2, 170);
    ui.itemListTable->setColumnWidth(3, 80);

    ui.itemListTable->verticalHeader()->setDefaultSectionSize(52);
    ui.itemListTable->horizontalHeader()->setFixedHeight(52);
}

