#include "AssignmentsPage.h"
#include "AsyncTask.h"
#include "Theme.h"
#include "AssignTemplateDialog.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>

namespace
{
    QString formatIsoDateTime(const std::string& value)
    {
        QString text = QString::fromStdString(value).trimmed();

        if (text.isEmpty()) {
            return "N/A";
        }

        text.replace("T", " ");

        int dotIndex = text.indexOf('.');
        if (dotIndex >= 0) {
            text = text.left(dotIndex);
        }

        if (text.endsWith("Z")) {
            text.chop(1);
        }

        if (text.length() >= 16) {
            return text.left(16);
        }

        return text;
    }
}

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
        ui.searchInput,
        &QLineEdit::textChanged,
        this,
        &AssignmentsPage::onSearchChanged
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
    ui.assignmentsTable->setColumnCount(6);

    ui.assignmentsTable->setHorizontalHeaderLabels(
        QStringList()
        << "Truck"
        << "Template"
        << "Assigned By"
        << "Assigned On"
        << "Status"
        << "Actions"
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
    ui.assignmentsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);

    ui.assignmentsTable->setColumnWidth(5, 170);
    ui.assignmentsTable->verticalHeader()->setDefaultSectionSize(52);

    ui.assignmentsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.assignmentsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void AssignmentsPage::loadAssignments()
{
    if (!truckStockService) {
        currentAssignments.clear();
        filteredAssignments.clear();

        populateTable();
        updatePagination();

        return;
    }

    TruckStockService* svc = truckStockService;

    AsyncTask::run(this,
        [svc]() { return svc->getAssignments(); },
        [this](auto assignments) {
            currentAssignments = assignments;
            filterAssignments();
        });
}

void AssignmentsPage::refreshAssignments()
{
    loadAssignments();
}

void AssignmentsPage::filterAssignments()
{
    QString searchText =
        ui.searchInput->text().trimmed();

    filteredAssignments.clear();

    for (const TruckAssignmentDto& assignment : currentAssignments) {
        QString truck =
            QString::fromStdString(assignment.truckNumber);

        QString templateName =
            QString::fromStdString(assignment.templateName);

        QString assignedBy =
            QString::fromStdString(assignment.assignedBy);

        QString assignedOn =
            QString::fromStdString(assignment.assignedOn);

        QString status =
            QString::fromStdString(assignment.status);

        bool matches =
            searchText.isEmpty() ||
            truck.contains(searchText, Qt::CaseInsensitive) ||
            templateName.contains(searchText, Qt::CaseInsensitive) ||
            assignedBy.contains(searchText, Qt::CaseInsensitive) ||
            assignedOn.contains(searchText, Qt::CaseInsensitive) ||
            status.contains(searchText, Qt::CaseInsensitive);

        if (matches) {
            filteredAssignments.push_back(assignment);
        }
    }

    currentPage = 1;

    populateTable();
    updatePagination();
}

void AssignmentsPage::setSearchText(
    const QString& text
)
{
    ui.searchInput->setText(text);
}

void AssignmentsPage::populateTable()
{
    ui.assignmentsTable->clearContents();
    ui.assignmentsTable->clearSpans();

    int totalItems =
        static_cast<int>(filteredAssignments.size());

    int startIndex =
        (currentPage - 1) * pageSize;

    int endIndex =
        (std::min)(startIndex + pageSize, totalItems);

    int rowCount =
        endIndex - startIndex;

    if (rowCount == 0) {
        ui.assignmentsTable->setRowCount(1);

        ui.assignmentsTable->setItem(
            0,
            0,
            new QTableWidgetItem("No assignments found")
        );

        ui.assignmentsTable->setSpan(0, 0, 1, 6);

        return;
    }

    ui.assignmentsTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const TruckAssignmentDto& assignment =
            filteredAssignments[startIndex + row];

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
                formatIsoDateTime(assignment.assignedOn)
            )
        );

        ui.assignmentsTable->setItem(
            row,
            4,
            new QTableWidgetItem(
                QString::fromStdString(assignment.status)
            )
        );

        addActionButtons(
            row,
            assignment
        );
    }
}

void AssignmentsPage::addActionButtons(
    int row,
    const TruckAssignmentDto& assignment
)
{
    QWidget* actionWidget =
        new QWidget(this);

    actionWidget->setObjectName("actionContainer");

    QHBoxLayout* layout =
        new QHBoxLayout(actionWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->setAlignment(Qt::AlignCenter);

    QPushButton* editButton =
        new QPushButton("✎", actionWidget);

    editButton->setObjectName("editButton");
    editButton->setToolTip("Edit Assignment");

    QPushButton* unassignButton =
        new QPushButton("🗑", actionWidget);

    unassignButton->setObjectName("unassignButton");
    unassignButton->setToolTip("Unassign Template");

    layout->addWidget(editButton);
    layout->addWidget(unassignButton);

    ui.assignmentsTable->setCellWidget(
        row,
        5,
        actionWidget
    );

    connect(
        editButton,
        &QPushButton::clicked,
        this,
        [this, assignment]() {
            onEditAssignmentClicked(assignment);
        }
    );

    connect(
        unassignButton,
        &QPushButton::clicked,
        this,
        [this, assignment]() {
            onUnassignClicked(assignment);
        }
    );
}

void AssignmentsPage::updatePagination()
{
    int totalItems =
        static_cast<int>(filteredAssignments.size());

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

            emit assignmentsChanged();
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

void AssignmentsPage::onUnassignClicked(
    const TruckAssignmentDto& assignment
)
{
    QMessageBox::StandardButton result =
        QMessageBox::question(
            this,
            "Unassign Template",
            QString("Remove template '%1' from truck '%2'?")
            .arg(QString::fromStdString(assignment.templateName))
            .arg(QString::fromStdString(assignment.truckNumber)),
            QMessageBox::Yes | QMessageBox::No
        );

    if (result != QMessageBox::Yes) {
        return;
    }

    if (!truckStockService) {
        QMessageBox::warning(
            this,
            "Error",
            "Truck stock service is unavailable."
        );

        return;
    }

    bool success =
        truckStockService->unassignTemplate(assignment.id);

    if (success) {
        QMessageBox::information(
            this,
            "Success",
            "Assignment removed successfully."
        );

        emit assignmentsChanged();

        loadAssignments();
    }
    else {
        QMessageBox::warning(
            this,
            "Error",
            "Failed to remove assignment."
        );
    }
}

void AssignmentsPage::onSearchChanged(
    const QString& text
)
{
    Q_UNUSED(text);

    filterAssignments();
}

void AssignmentsPage::onPreviousPageClicked()
{
    if (currentPage > 1) {
        currentPage--;

        populateTable();
        updatePagination();
    }
}

void AssignmentsPage::onEditAssignmentClicked(
    const TruckAssignmentDto& assignment
)
{
    AssignTemplateDialog dialog(
        truckStockService,
        userService,
        this
    );

    dialog.setEditMode(
        QString::fromStdString(assignment.truckId),
        QString::fromStdString(assignment.templateId)
    );

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    if (!truckStockService) {
        QMessageBox::warning(
            this,
            "Error",
            "Truck stock service is unavailable."
        );

        return;
    }

    UpdateAssignmentRequest request;

    request.truckId =
        dialog.getTruckId().toStdString();

    request.templateId =
        dialog.getTemplateId().toStdString();

    request.status =
        assignment.status;

    bool success =
        truckStockService->updateAssignment(
            assignment.id,
            request
        );

    if (success) {
        QMessageBox::information(
            this,
            "Success",
            "Assignment updated successfully."
        );

        emit assignmentsChanged();

        loadAssignments();
    }
    else {
        QMessageBox::warning(
            this,
            "Error",
            "Failed to update assignment."
        );
    }
}

void AssignmentsPage::onNextPageClicked()
{
    int totalItems =
        static_cast<int>(filteredAssignments.size());

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
        static_cast<int>(filteredAssignments.size());

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