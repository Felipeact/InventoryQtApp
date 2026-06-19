#include "FleetPage.h"

#include "TrucksPage.h"
#include "StockTemplatesPage.h"
#include "AssignmentsPage.h"

#include <QTabWidget>
#include <QVBoxLayout>

FleetPage::FleetPage(
    TruckStockService* truckStockService,
    UserService* userService,
    const std::vector<std::string>& permissions,
    QWidget* parent
)
    : QWidget(parent)
{
    setObjectName("FleetPageClass");

    // No outer margins: the child pages already provide their own padding, so
    // this keeps their layout identical to when they were shown full-bleed.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    tabs_ = new QTabWidget(this);
    tabs_->setObjectName("fleetTabs");
    tabs_->setDocumentMode(true);

    // TrucksPage carries its own write-action gating (MANAGE_TRUCK_STOCK), so
    // forward the permissions array to it.
    trucksPage_ = new TrucksPage(truckStockService, userService, permissions, this);
    templatesPage_ = new StockTemplatesPage(truckStockService, this);
    assignmentsPage_ = new AssignmentsPage(truckStockService, userService, this);

    tabs_->addTab(trucksPage_, "Trucks");
    tabs_->addTab(templatesPage_, "Templates");
    tabs_->addTab(assignmentsPage_, "Assignments");

    layout->addWidget(tabs_);
}

void FleetPage::showTrucks()
{
    if (tabs_ && trucksPage_) {
        tabs_->setCurrentWidget(trucksPage_);
    }
}

void FleetPage::showTemplates()
{
    if (tabs_ && templatesPage_) {
        tabs_->setCurrentWidget(templatesPage_);
    }
}

void FleetPage::showAssignments()
{
    if (tabs_ && assignmentsPage_) {
        tabs_->setCurrentWidget(assignmentsPage_);
    }
}

void FleetPage::applyTheme(Theme::AppTheme theme)
{
    if (trucksPage_) {
        trucksPage_->applyTheme(theme);
    }

    if (templatesPage_) {
        templatesPage_->applyTheme(theme);
    }

    if (assignmentsPage_) {
        assignmentsPage_->applyTheme(theme);
    }
}
