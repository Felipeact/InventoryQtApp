#include "ItemsPage.h"

#include <QTableWidgetItem>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QWidget>
#include <QFrame>

ItemsPage::ItemsPage(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setupTable();
    loadFakeData();
}

ItemsPage::~ItemsPage()
{
}

void ItemsPage::setupTable()
{
    ui.itemsTable->setColumnCount(9);

    ui.itemsTable->setHorizontalHeaderLabels(
        QStringList()
        << "Item Name"
        << "Image"
        << "Model"
        << "Type"
        << "Store"
        << "Amount"
        << "Project"
        << "Account"
        << "Action"
    );

    ui.itemsTable->verticalHeader()->setVisible(false);
    ui.itemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.itemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.itemsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.itemsTable->setShowGrid(false);
    ui.itemsTable->setFrameShape(QFrame::NoFrame);
    ui.itemsTable->setFocusPolicy(Qt::NoFocus);

    ui.itemsTable->horizontalHeader()->setHighlightSections(false);
    ui.itemsTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui.itemsTable->horizontalHeader()->setFixedHeight(48);

    ui.itemsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.itemsTable->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Fixed);
    ui.itemsTable->setColumnWidth(8, 130);

    ui.itemsTable->verticalHeader()->setDefaultSectionSize(52);

    ui.itemsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.itemsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    const QString premiumScrollBarStyle = R"(
    QScrollBar:vertical {
        background: transparent;
        width: 6px;
        margin: 4px 0 4px 0;
    }

    QScrollBar::handle:vertical {
        background: rgba(140, 140, 140, 0.35);
        border-radius: 3px;
        min-height: 30px;
    }

    QScrollBar::handle:vertical:hover {
        background: rgba(180, 180, 180, 0.65);
    }

    QScrollBar::handle:vertical:pressed {
        background: rgba(200, 200, 200, 0.9);
    }

    QScrollBar::add-line:vertical,
    QScrollBar::sub-line:vertical {
        height: 0;
    }

    QScrollBar::add-page:vertical,
    QScrollBar::sub-page:vertical {
        background: transparent;
    }

    QScrollBar:horizontal {
        background: transparent;
        height: 6px;
        margin: 0 4px 0 4px;
    }

    QScrollBar::handle:horizontal {
        background: rgba(140, 140, 140, 0.35);
        border-radius: 3px;
        min-width: 30px;
    }

    QScrollBar::handle:horizontal:hover {
        background: rgba(180, 180, 180, 0.65);
    }

    QScrollBar::handle:horizontal:pressed {
        background: rgba(200, 200, 200, 0.9);
    }

    QScrollBar::add-line:horizontal,
    QScrollBar::sub-line:horizontal {
        width: 0;
    }

    QScrollBar::add-page:horizontal,
    QScrollBar::sub-page:horizontal {
        background: transparent;
    }
    )";

    ui.itemsTable->verticalScrollBar()->setStyleSheet(premiumScrollBarStyle);
    ui.itemsTable->horizontalScrollBar()->setStyleSheet(premiumScrollBarStyle);

}

void ItemsPage::loadFakeData()
{
    const int rowCount = 30;
    ui.itemsTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        ui.itemsTable->setItem(row, 0, new QTableWidgetItem(row == 0 ? "Gas Kitting" : "Condet"));
        ui.itemsTable->setItem(row, 1, new QTableWidgetItem("Image"));
        ui.itemsTable->setItem(row, 2, new QTableWidgetItem(row == 1 ? "Co-7898" : "G-7893"));
        ui.itemsTable->setItem(row, 3, new QTableWidgetItem("IE Project Items"));
        ui.itemsTable->setItem(row, 4, new QTableWidgetItem(row == 0 ? "22 House Store" : "HQ Main Store"));
        ui.itemsTable->setItem(row, 5, new QTableWidgetItem(row == 0 ? "1 pcs" : "5 pcs"));
        ui.itemsTable->setItem(row, 6, new QTableWidgetItem("HQ"));
        ui.itemsTable->setItem(row, 7, new QTableWidgetItem(row % 3 == 0 ? "Need Invitation" : "Activated"));

        QWidget* actionWidget = new QWidget();
        actionWidget->setStyleSheet("background-color: transparent;");

        QHBoxLayout* actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 0, 0);
        actionLayout->setSpacing(6);

        QPushButton* viewButton = new QPushButton("👁");
        QPushButton* editButton = new QPushButton("✎");
        QPushButton* deleteButton = new QPushButton("🗑");

        viewButton->setObjectName("viewActionButton");
        editButton->setObjectName("editActionButton");
        deleteButton->setObjectName("deleteActionButton");

        viewButton->setFixedSize(26, 26);
        editButton->setFixedSize(26, 26);
        deleteButton->setFixedSize(26, 26);

        viewButton->setStyleSheet(
            "QPushButton { background-color: #f59e0b; color: white; border: none; border-radius: 6px; }"
        );

        editButton->setStyleSheet(
            "QPushButton { background-color: #0078d4; color: white; border: none; border-radius: 6px; }"
        );

        deleteButton->setStyleSheet(
            "QPushButton { background-color: #dc2626; color: white; border: none; border-radius: 6px; }"
        );

        actionLayout->addWidget(viewButton);
        actionLayout->addWidget(editButton);
        actionLayout->addWidget(deleteButton);
        actionLayout->addStretch();

        ui.itemsTable->setCellWidget(row, 8, actionWidget);
    }
}