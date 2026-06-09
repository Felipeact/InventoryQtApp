#include "TruckStockHistoryDialog.h"
#include "Theme.h"

#include <QAbstractItemView>
#include <QFrame>
#include <QHeaderView>
#include <QMouseEvent>
#include <QPushButton>
#include <QTableWidgetItem>

TruckStockHistoryDialog::TruckStockHistoryDialog(
    const MyTruckStockItemDto& item,
    const std::vector<TruckStockMovementDto>& movements,
    QWidget* parent
)
    : QDialog(parent),
    item(item),
    movements(movements)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setModal(true);

    resize(860, 560);
    setMinimumSize(860, 560);
    setMaximumSize(860, 560);

    applyTheme(Theme::AppTheme::Dark);

    setupTable();
    setupConnections();
    populateTable();
}

TruckStockHistoryDialog::~TruckStockHistoryDialog()
{
}

void TruckStockHistoryDialog::setupConnections()
{
    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &TruckStockHistoryDialog::onCloseClicked
    );

    connect(
        ui.closeBottomButton,
        &QPushButton::clicked,
        this,
        &TruckStockHistoryDialog::onCloseClicked
    );
}

void TruckStockHistoryDialog::setupTable()
{
    ui.historyTable->setColumnCount(6);

    ui.historyTable->setHorizontalHeaderLabels(
        QStringList()
        << "Date"
        << "Action"
        << "Previous"
        << "New"
        << "Changed By"
        << "Notes"
    );

    ui.historyTable->verticalHeader()->setVisible(false);
    ui.historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.historyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.historyTable->setShowGrid(false);
    ui.historyTable->setFrameShape(QFrame::NoFrame);
    ui.historyTable->setFocusPolicy(Qt::NoFocus);
    ui.historyTable->setAlternatingRowColors(false);
    ui.historyTable->viewport()->setAutoFillBackground(false);

    ui.historyTable->horizontalHeader()->setHighlightSections(false);
    ui.historyTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    ui.historyTable->horizontalHeader()->setFixedHeight(44);
    ui.historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui.historyTable->verticalHeader()->setDefaultSectionSize(44);

    ui.historyTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.historyTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void TruckStockHistoryDialog::populateTable()
{
    ui.titleLabel->setText(
        "Stock History - " + QString::fromStdString(item.productName)
    );

    ui.itemNameValue->setText(
        QString::fromStdString(item.productName)
    );

    ui.currentQtyValue->setText(
        QString::number(item.currentQuantity)
    );

    ui.statusValue->setText(
        QString::fromStdString(item.status)
    );

    std::vector<TruckStockMovementDto> itemMovements;

    for (const TruckStockMovementDto& movement : movements) {
        if (movement.truckStockItemId == item.id) {
            itemMovements.push_back(movement);
        }
    }

    ui.historyCountValue->setText(
        QString::number(static_cast<int>(itemMovements.size()))
    );

    if (itemMovements.empty()) {
        ui.historyTable->setRowCount(1);

        ui.historyTable->setItem(
            0,
            0,
            new QTableWidgetItem("No history found for this item")
        );

        ui.historyTable->setSpan(0, 0, 1, 6);

        return;
    }

    ui.historyTable->setRowCount(
        static_cast<int>(itemMovements.size())
    );

    for (int row = 0; row < static_cast<int>(itemMovements.size()); row++) {
        const TruckStockMovementDto& movement =
            itemMovements[row];

        ui.historyTable->setItem(
            row,
            0,
            new QTableWidgetItem(formatDate(movement.createdAt))
        );

        ui.historyTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(movement.action))
        );

        ui.historyTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::number(movement.previousQuantity))
        );

        ui.historyTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::number(movement.newQuantity))
        );

        ui.historyTable->setItem(
            row,
            4,
            new QTableWidgetItem(QString::fromStdString(movement.changedBy))
        );

        ui.historyTable->setItem(
            row,
            5,
            new QTableWidgetItem(QString::fromStdString(movement.notes))
        );
    }
}

QString TruckStockHistoryDialog::formatDate(
    const std::string& value
) const
{
    QString date =
        QString::fromStdString(value);

    if (date.length() >= 10) {
        return date.left(10);
    }

    return date;
}

void TruckStockHistoryDialog::onCloseClicked()
{
    reject();
}

void TruckStockHistoryDialog::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

void TruckStockHistoryDialog::mousePressEvent(
    QMouseEvent* event
)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition =
            event->globalPosition().toPoint()
            - frameGeometry().topLeft();

        event->accept();
    }
}

void TruckStockHistoryDialog::mouseMoveEvent(
    QMouseEvent* event
)
{
    if (event->buttons() & Qt::LeftButton) {
        move(
            event->globalPosition().toPoint()
            - dragPosition
        );

        event->accept();
    }
}