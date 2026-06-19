#pragma once

#include <QWidget>
#include <vector>
#include <string>

#include "Theme.h"

class QTabWidget;
class TruckStockService;
class UserService;
class TrucksPage;
class StockTemplatesPage;
class AssignmentsPage;

// Unified "Fleet" page. Groups the three truck-stock management screens
// (Trucks, Templates, Assignments) under a single tabbed view, matching the
// web app's Fleet page. FleetPage owns the three child pages; callers reach
// them through the getters to wire refresh signals and global-search jumps.
//
// Intentionally has no Q_OBJECT / signals: it is a passive container, so it
// needs no moc step and adds no risk to the build.
class FleetPage : public QWidget
{
public:
    FleetPage(
        TruckStockService* truckStockService,
        UserService* userService,
        const std::vector<std::string>& permissions,
        QWidget* parent = nullptr
    );

    TrucksPage* trucksPage() const { return trucksPage_; }
    StockTemplatesPage* templatesPage() const { return templatesPage_; }
    AssignmentsPage* assignmentsPage() const { return assignmentsPage_; }

    // Select a tab by the screen it hosts (no-op if that tab is absent).
    void showTrucks();
    void showTemplates();
    void showAssignments();

    void applyTheme(Theme::AppTheme theme);

private:
    QTabWidget* tabs_ = nullptr;
    TrucksPage* trucksPage_ = nullptr;
    StockTemplatesPage* templatesPage_ = nullptr;
    AssignmentsPage* assignmentsPage_ = nullptr;
};
