#include "TruckDetailsDialog.h"
#include "Theme.h"

#include <QAbstractItemView>
#include <QFrame>
#include <QHeaderView>
#include <QMouseEvent>
#include <QPushButton>
#include <QTableWidgetItem>

TruckDetailsDialog::TruckDetailsDialog(
    const TruckDto& truck,
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QDialog(parent),
    truck(truck),
    truckStockService(truckStockService)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);

    resize(820, 620);
    setMinimumSize(820, 620);
    setMaximumSize(820, 620);

    applyTheme(Theme::AppTheme::Dark);

    setupTables();
    setupConnections();
    loadTruckDetails();
}

TruckDetailsDialog::~TruckDetailsDialog()
{
}

void TruckDetailsDialog::setupConnections()
{
    connect(
        ui.closeButton,
        &QPushButton::clicked,
        this,
        &TruckDetailsDialog::onCloseClicked
    );

    connect(
        ui.closeBottomButton,
        &QPushButton::clicked,
        this,
        &TruckDetailsDialog::onCloseClicked
    );
}

void TruckDetailsDialog::setupTables()
{
    ui.assignmentsTable->setColumnCount(4);

    ui.assignmentsTable->setHorizontalHeaderLabels(
        QStringList()
        << "Template"
        << "Assigned By"
        << "Assigned On"
        << "Status"
    );

    ui.lowStockTable->setColumnCount(5);

    ui.lowStockTable->setHorizontalHeaderLabels(
        QStringList()
        << "Item"
        << "Template"
        << "Current"
        << "Minimum"
        << "Status"
    );

    QList<QTableWidget*> tables = {
        ui.assignmentsTable,
        ui.lowStockTable
    };

    for (QTableWidget* table : tables) {
        table->verticalHeader()->setVisible(false);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setShowGrid(false);
        table->setFrameShape(QFrame::NoFrame);
        table->setFocusPolicy(Qt::NoFocus);
        table->setAlternatingRowColors(false);
        table->viewport()->setAutoFillBackground(false);

        table->horizontalHeader()->setHighlightSections(false);
        table->horizontalHeader()->setDefaultAlignment(
            Qt::AlignLeft | Qt::AlignVCenter
        );

        table->horizontalHeader()->setFixedHeight(38);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        table->verticalHeader()->setDefaultSectionSize(38);

        table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}

void TruckDetailsDialog::loadTruckDetails()
{
    ui.titleLabel->setText("Truck Details");

    ui.truckNumberValue->setText(
        QString::fromStdString(truck.truckName)
    );

    ui.plateValue->setText(
        QString::fromStdString(truck.licensePlate)
    );

    ui.technicianValue->setText(
        QString::fromStdString(truck.technicianName)
    );

    ui.statusValue->setText(
        QString::fromStdString(truck.status)
    );

    loadAssignments();
    loadLowStockItems();
}

void TruckDetailsDialog::loadAssignments()
{
    ui.assignmentsTable->clearContents();
    ui.assignmentsTable->clearSpans();

    if (!truckStockService) {
        ui.assignmentsTable->setRowCount(1);
        ui.assignmentsTable->setItem(
            0,
            0,
            new QTableWidgetItem("Truck stock service unavailable")
        );
        ui.assignmentsTable->setSpan(0, 0, 1, 4);
        return;
    }

    std::vector<TruckAssignmentDto> assignments =
        truckStockService->getAssignments();

    std::vector<TruckAssignmentDto> truckAssignments;

    for (const TruckAssignmentDto& assignment : assignments) {
        if (
            QString::fromStdString(assignment.truckNumber)
            .compare(
                QString::fromStdString(truck.truckName),
                Qt::CaseInsensitive
            ) == 0
            ) {
            truckAssignments.push_back(assignment);
        }
    }

    if (truckAssignments.empty()) {
        ui.assignmentsTable->setRowCount(1);
        ui.assignmentsTable->setItem(
            0,
            0,
            new QTableWidgetItem("No assigned templates found")
        );
        ui.assignmentsTable->setSpan(0, 0, 1, 4);
        return;
    }

    ui.assignmentsTable->setRowCount(
        static_cast<int>(truckAssignments.size())
    );

    for (int row = 0; row < static_cast<int>(truckAssignments.size()); row++) {
        const TruckAssignmentDto& assignment =
            truckAssignments[row];

        ui.assignmentsTable->setItem(
            row,
            0,
            new QTableWidgetItem(
                QString::fromStdString(assignment.templateName)
            )
        );

        ui.assignmentsTable->setItem(
            row,
            1,
            new QTableWidgetItem(
                QString::fromStdString(assignment.assignedBy)
            )
        );

        ui.assignmentsTable->setItem(
            row,
            2,
            new QTableWidgetItem(
                QString::fromStdString(assignment.assignedOn)
            )
        );

        ui.assignmentsTable->setItem(
            row,
            3,
            new QTableWidgetItem(
                QString::fromStdString(assignment.status)
            )
        );
    }
}

void TruckDetailsDialog::loadLowStockItems()
{
    ui.lowStockTable->clearContents();
    ui.lowStockTable->clearSpans();

    if (!truckStockService) {
        ui.lowStockTable->setRowCount(1);
        ui.lowStockTable->setItem(
            0,
            0,
            new QTableWidgetItem("Truck stock service unavailable")
        );
        ui.lowStockTable->setSpan(0, 0, 1, 5);
        return;
    }

    std::vector<LowStockItemDto> lowStockItems =
        truckStockService->getLowStockItems();

    std::vector<LowStockItemDto> truckLowStockItems;

    for (const LowStockItemDto& item : lowStockItems) {
        if (
            QString::fromStdString(item.truckNumber)
            .compare(
                QString::fromStdString(truck.truckName),
                Qt::CaseInsensitive
            ) == 0
            ) {
            truckLowStockItems.push_back(item);
        }
    }

    ui.lowStockCountValue->setText(
        QString::number(
            static_cast<int>(truckLowStockItems.size())
        )
    );

    if (truckLowStockItems.empty()) {
        ui.lowStockTable->setRowCount(1);
        ui.lowStockTable->setItem(
            0,
            0,
            new QTableWidgetItem("No low stock items for this truck")
        );
        ui.lowStockTable->setSpan(0, 0, 1, 5);
        return;
    }

    ui.lowStockTable->setRowCount(
        static_cast<int>(truckLowStockItems.size())
    );

    for (int row = 0; row < static_cast<int>(truckLowStockItems.size()); row++) {
        const LowStockItemDto& item =
            truckLowStockItems[row];

        ui.lowStockTable->setItem(
            row,
            0,
            new QTableWidgetItem(
                QString::fromStdString(item.productName)
            )
        );

        ui.lowStockTable->setItem(
            row,
            1,
            new QTableWidgetItem(
                QString::fromStdString(item.templateName)
            )
        );

        ui.lowStockTable->setItem(
            row,
            2,
            new QTableWidgetItem(
                QString::number(item.currentQuantity)
            )
        );

        ui.lowStockTable->setItem(
            row,
            3,
            new QTableWidgetItem(
                QString::number(item.minimumQuantity)
            )
        );

        ui.lowStockTable->setItem(
            row,
            4,
            new QTableWidgetItem(
                QString::fromStdString(item.status)
            )
        );
    }
}

void TruckDetailsDialog::onCloseClicked()
{
    reject();
}

void TruckDetailsDialog::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::dialogStyle(theme)
    );
}

void TruckDetailsDialog::mousePressEvent(
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

void TruckDetailsDialog::mouseMoveEvent(
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