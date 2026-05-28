#include "AssignmentsPage.h"
#include "AssignTemplateDialog.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <algorithm>

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

    setupConnections();
    loadAssignments();
}

AssignmentsPage::~AssignmentsPage()
{
}

void AssignmentsPage::setupConnections()
{
    connect(ui.assignTemplateButton, &QPushButton::clicked,
        this, &AssignmentsPage::onAssignTemplateClicked);

    connect(ui.pageButton, &QPushButton::clicked,
        this, &AssignmentsPage::onPreviousPageClicked);

    connect(ui.pageButton_3, &QPushButton::clicked,
        this, &AssignmentsPage::onNextPageClicked);

    connect(ui.pageButton_2, &QPushButton::clicked,
        this, &AssignmentsPage::onPage2Clicked);
}

void AssignmentsPage::loadAssignments()
{
    ui.assignmentsTable->clearContents();

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

    ui.assignmentsTable->horizontalHeader()->setStretchLastSection(true);
    ui.assignmentsTable->verticalHeader()->setVisible(false);
}

void AssignmentsPage::refreshAssignments()
{
    loadAssignments();
}

void AssignmentsPage::populateTable()
{
    ui.assignmentsTable->clearContents();
    ui.assignmentsTable->setColumnCount(5);

    int totalItems =
        static_cast<int>(currentAssignments.size());

    int startIndex =
        (currentPage - 1) * pageSize;

    int endIndex =
        (std::min)(startIndex + pageSize, totalItems);

    int rowCount =
        endIndex - startIndex;

    ui.assignmentsTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; ++row) {
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