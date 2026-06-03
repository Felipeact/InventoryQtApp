#include "AssignmentsPage.h"
#include "Theme.h"
#include "AssignTemplateDialog.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QFrame>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>

AssignmentsPage::AssignmentsPage(
    TruckStockService* truckStockService,
    UserService* userService,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService),
    userService(userService)
{
    ui.setupUi(this);

    applyTheme(Theme::AppTheme::Dark);

    setupTable();
    setupConnections();
    loadAssignments();
}

AssignmentsPage::~AssignmentsPage()
{
}

void AssignmentsPage::setupConnections()
{
    connect(
        ui.assignTemplateButton,
        &QPushButton::clicked,
        this,
        &AssignmentsPage::onAssignTemplateClicked
    );

    connect(
        ui.pageButton,
        &QPushButton::clicked,
        this,
        &AssignmentsPage::onPreviousPageClicked
    );

    connect(
        ui.pageButton_3,
        &QPushButton::clicked,
        this,
        &AssignmentsPage::onNextPageClicked
    );

    connect(
        ui.pageButton_2,
        &QPushButton::clicked,
        this,
        &AssignmentsPage::onPage2Clicked
    );
}

void AssignmentsPage::setupTable()
{
    ui.assignmentsTable->setColumnCount(5);

    ui.assignmentsTable->setHorizontalHeaderLabels(
        QStringList()
        << "Truck"
        << "Template"
        << "Assigned By"
        << "Assigned On"
        << "Status"
    );

    ui.assignmentsTable->verticalHeader()->setVisible(false);
    ui.assignmentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.assignmentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.assignmentsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.assignmentsTable->setShowGrid(false);
    ui.assignmentsTable->setFrameShape(QFrame::NoFrame);
    ui.assignmentsTable->setFocusPolicy(Qt::NoFocus);
    ui.assignmentsTable->setAlternatingRowColors(false);
    ui.assignmentsTable->viewport()->setAutoFillBackground(false);

    ui.assignmentsTable->horizontalHeader()->setHighlightSections(false);
    ui.assignmentsTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    ui.assignmentsTable->horizontalHeader()->setFixedHeight(48);
    ui.assignmentsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui.assignmentsTable->verticalHeader()->setDefaultSectionSize(52);

    ui.assignmentsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.assignmentsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void AssignmentsPage::loadAssignments()
{
    if (!truckStockService) {
        currentAssignments.clear();

        populateTable();
        updatePagination();

        return;
    }

    currentAssignments =
        truckStockService->getAssignments();

    currentPage = 1;

    populateTable();
    updatePagination();
}

void AssignmentsPage::refreshAssignments()
{
    loadAssignments();
}

void AssignmentsPage::populateTable()
{
    ui.assignmentsTable->clearContents();

    int totalItems =
        static_cast<int>(currentAssignments.size());

    int startIndex =
        (currentPage - 1) * pageSize;

    int endIndex =
        (std::min)(startIndex + pageSize, totalItems);

    int rowCount =
        endIndex - startIndex;

    ui.assignmentsTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const TruckAssignmentDto& assignment =
            currentAssignments[startIndex + row];

        ui.assignmentsTable->setItem(
            row,
            0,
            new QTableWidgetItem(
                QString::fromStdString(assignment.truckNumber)
            )
        );

        ui.assignmentsTable->setItem(
            row,
            1,
            new QTableWidgetItem(
                QString::fromStdString(assignment.templateName)
            )
        );

        ui.assignmentsTable->setItem(
            row,
            2,
            new QTableWidgetItem(
                QString::fromStdString(assignment.assignedBy)
            )
        );

        ui.assignmentsTable->setItem(
            row,
            3,
            new QTableWidgetItem(
                QString::fromStdString(assignment.assignedOn)
            )
        );

        ui.assignmentsTable->setItem(
            row,
            4,
            new QTableWidgetItem(
                QString::fromStdString(assignment.status)
            )
        );
    }
}

void AssignmentsPage::updatePagination()
{
    int totalItems =
        static_cast<int>(currentAssignments.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage > totalPages) {
        currentPage = totalPages;
    }

    int startItem =
        totalItems == 0
        ? 0
        : ((currentPage - 1) * pageSize) + 1;

    int endItem =
        (std::min)(currentPage * pageSize, totalItems);

    ui.paginationLabel->setText(
        QString("Showing %1 to %2 of %3 assignments")
        .arg(startItem)
        .arg(endItem)
        .arg(totalItems)
    );

    ui.activePageButton->setText(
        QString::number(currentPage)
    );

    ui.pageButton_2->setText(
        QString::number(currentPage + 1)
    );

    ui.pageButton->setEnabled(
        currentPage > 1
    );

    ui.pageButton_3->setEnabled(
        currentPage < totalPages
    );

    ui.pageButton_2->setVisible(
        currentPage < totalPages
    );
}

void AssignmentsPage::onAssignTemplateClicked()
{
    AssignTemplateDialog dialog(
        truckStockService,
        userService,
        this
    );

    if (dialog.exec() == QDialog::Accepted) {
        CreateAssignmentRequest request;

        request.truckId =
            dialog.getTruckId().toStdString();

        request.templateId =
            dialog.getTemplateId().toStdString();

        bool success =
            truckStockService->assignTemplate(request);

        if (success) {
            QMessageBox::information(
                this,
                "Success",
                "Template assigned successfully."
            );

            loadAssignments();
        }
        else {
            QMessageBox::warning(
                this,
                "Error",
                "Failed to assign template."
            );
        }
    }
}

void AssignmentsPage::onPreviousPageClicked()
{
    if (currentPage > 1) {
        currentPage--;

        populateTable();
        updatePagination();
    }
}

void AssignmentsPage::onNextPageClicked()
{
    int totalItems =
        static_cast<int>(currentAssignments.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage < totalPages) {
        currentPage++;

        populateTable();
        updatePagination();
    }
}

void AssignmentsPage::onPage2Clicked()
{
    int totalItems =
        static_cast<int>(currentAssignments.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage + 1 <= totalPages) {
        currentPage++;

        populateTable();
        updatePagination();
    }
}

void AssignmentsPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::assignmentsPageStyle(theme)
    );
}