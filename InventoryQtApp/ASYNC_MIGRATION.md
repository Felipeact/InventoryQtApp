# Non-blocking API calls — status & migration guide

## The problem

Service calls (`productService.getProducts()`, etc.) are **synchronous**: they
block the calling thread for the full duration of the HTTP request (up to the
5 s timeout). When invoked on the GUI thread, the UI freezes for that time.

## Actual state (survey)

Threading is currently **inconsistent** across the UI:

- **Already offloaded** to a worker thread (do not freeze): `ItemsPage`,
  `AssetsPage`, `DashboardPage`, `ReportsPage`, `TruckStockDashboardPage`,
  `TrucksPage`, `ReceiptsPage`, `UsersPage` — but typically only their *main
  refresh*; secondary actions (create/update/delete) often still run on the GUI
  thread.
- **Fully blocking** the GUI thread (highest priority to fix):
  `GlobalSearchDialog` (8 call sites), `StockTemplatesPage` (6),
  `AssignmentsPage`, `LowStockAlertsPage`, `MyTruckStockPage`, `ScanPage`,
  `AssignTemplateDialog`, `NotificationDialog`, `TruckDetailsDialog`,
  `SettingsPage`, `UploadReceiptDialog`, `AddEditTruckDialog`.

The pages that *do* offload use a verbose hand-rolled pattern
(`QThread::create` + `QPointer` + `QMetaObject::invokeMethod`) that spawns a new
OS thread per call.

## The primitive

`AsyncTask::run` (`AsyncTask.h`) replaces both the blocking calls and the
hand-rolled threading. It runs work on Qt's pooled threads and delivers the
result on the caller's (GUI) thread, and is a no-op if the page is destroyed
first. It is unit-tested in `tests/tst_asynctask.cpp`.

```cpp
#include "AsyncTask.h"

void ItemsPage::refreshProducts()
{
    AsyncTask::run(this,
        [&svc = productService]() { return svc.getProducts(true); }, // worker thread
        [this](json products) {                                      // GUI thread
            currentProducts = products;
            applyFilters();
        });
}
```

(`AsyncApi` is a thinner, typed variant for code that holds an `IApiClient`
directly and works in terms of `HttpResponse`.)

## Migration recipe

1. `#include "AsyncTask.h"`.
2. Move the blocking service call into the `work` lambda; capture the service by
   reference (it is owned by the window and outlives the page) — **not** widgets.
3. Move the UI update into the `onResult` lambda; it runs on the GUI thread, so
   touching widgets is safe. No `QPointer` needed (the callback is skipped if
   the page is gone).
4. For actions, disable the triggering control before the call and re-enable it
   in the callback so the user gets feedback and can't double-submit.

## Done

Non-modal pages — their primary load/refresh (and `ScanPage`'s submit) now run
through `AsyncTask::run` and no longer block the GUI thread:

- `ItemsPage` (`loadProducts`, `refreshProducts`)
- `StockTemplatesPage` (`loadTemplates`)
- `MyTruckStockPage` (`loadStock`)
- `LowStockAlertsPage` (`loadAlerts`)
- `AssignmentsPage` (`loadAssignments`)
- `ScanPage` (`onSubmitClicked` — scan + name lookup off-thread)
- `GlobalSearchDialog` (`runSearch` — gathers 8 sources off-thread, builds the
  tree on the GUI thread, with a generation guard against stale keystrokes)

## Intentionally left synchronous

**Modal dialogs** (`AssignTemplateDialog`, `AddEditTruckDialog`,
`UploadReceiptDialog`, `AddEditUserDialog`, …) keep their on-open combo loads
synchronous. They block interaction by design, open with only a brief bounded
pause, and several preselect combo values in `setEditMode` immediately after
loading — making them sensitive to load ordering. The risk/benefit there does
not favor async.

## Remaining (optional)

Secondary write actions (create/update/delete) on the list pages still run on
the GUI thread. They are single, user-initiated operations; convert them with
the `AsyncTask::run` recipe above plus a disable-control-during-call guard if
their momentary pause becomes a concern.
