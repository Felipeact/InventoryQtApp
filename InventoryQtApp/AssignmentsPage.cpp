#include "AssignmentsPage.h"
#include "AssignTemplateDialog.h"

#include <QMessageBox>
#include <QPushButton>
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
}

void AssignmentsPage::loadAssignments()
{
    ui.assignmentsTable->clearContents();

    std::vector<TruckAssignmentDto> assignments =
        truckStockService->getAssignments();

    ui.assignmentsTable->setRowCount(
        static_cast<int>(assignments.size())
    );

    ui.assignmentsTable->setColumnCount(5);

    for (int row = 0; row < static_cast<int>(assignments.size()); ++row) {

        const TruckAssignmentDto& assignment =
            assignments[row];

        ui.assignmentsTable->setItem(
            row,
            0,
            new QTableWidgetItem(
                QString::fromStdString(
                    assignment.truckNumber
                )
            )
        );

        ui.assignmentsTable->setItem(
            row,
            1,
            new QTableWidgetItem(
                QString::fromStdString(
                    assignment.templateName
                )
            )
        );

        ui.assignmentsTable->setItem(
            row,
            2,
            new QTableWidgetItem(
                QString::fromStdString(
                    assignment.assignedBy
                )
            )
        );

        ui.assignmentsTable->setItem(
            row,
            3,
            new QTableWidgetItem(
                QString::fromStdString(
                    assignment.assignedOn
                )
            )
        );

        ui.assignmentsTable->setItem(
            row,
            4,
            new QTableWidgetItem(
                QString::fromStdString(
                    assignment.status
                )
            )
        );
    }

    ui.paginationLabel->setText(
        QString("Showing %1 assignments")
        .arg(assignments.size())
    );
}

void AssignmentsPage::refreshAssignments()
{
    loadAssignments();
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
                "Failed to assign template. Check backend console."
            );
        }
    }
}

void AssignmentsPage::onPageChanged(int page)
{
    Q_UNUSED(page);
}