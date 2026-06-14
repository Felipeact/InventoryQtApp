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

## Done so far

- `ItemsPage::refreshProducts` and `ItemsPage::loadProducts` converted to
  `AsyncTask::run` (replacing the raw-`QThread` pattern) as the reference
  implementation.

## Recommended order for the rest

`GlobalSearchDialog` → `StockTemplatesPage` → `MyTruckStockPage` /
`LowStockAlertsPage` / `ScanPage` → remaining dialogs → secondary
(create/update/delete) actions on the list pages.
