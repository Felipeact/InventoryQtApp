#include "AssignmentsPage.h"

AssignmentsPage::AssignmentsPage(QWidget* parent)
    : QWidget(parent)
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
    connect(ui.assignTemplateButton, &QPushButton::clicked, this, &AssignmentsPage::onAssignTemplateClicked);
}

void AssignmentsPage::loadAssignments()
{
    // Load assignments from backend
}

void AssignmentsPage::refreshAssignments()
{
    loadAssignments();
}

void AssignmentsPage::onAssignTemplateClicked()
{
    // Open assign template dialog
}

void AssignmentsPage::onPageChanged(int page)
{
    // Load assignments for specified page
}
