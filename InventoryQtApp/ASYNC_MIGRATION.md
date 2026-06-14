# Migrating UI pages to non-blocking API calls

## Why

Every `ApiClient`/service call is **synchronous** and uses `cpr`, which blocks
the calling thread until the HTTP request completes (or times out, currently
5 s). When these are invoked directly from the GUI thread — which is how the
pages do it today — the entire UI freezes while the request is in flight. On a
slow or unreachable server this is a multi-second hang per action.

`AsyncApi` (`AsyncApi.h`) removes the freeze: it runs the call on a background
thread and delivers the result to a callback **on the GUI thread**, so widget
code stays simple and thread-safe.

## The pattern

Before (blocking — freezes the UI):

```cpp
void ItemsPage::refreshProducts()
{
    json products = productService.getProducts();   // blocks the GUI thread
    populateTable(products);
}
```

After (non-blocking):

```cpp
// Construct once, parented to the page so it lives on the GUI thread:
//   asyncApi_ = new AsyncApi(apiClient, this);

void ItemsPage::refreshProducts()
{
    setBusy(true);
    asyncApi_->get("/products?page=1&limit=100", [this](const HttpResponse& res) {
        setBusy(false);                  // runs on the GUI thread
        if (res.status_code != 200) {
            showError(ErrorHandler::getErrorMessage(res));
            return;
        }
        populateTable(json::parse(res.text, nullptr, false));
    });
}
```

## Guidelines

- Create one `AsyncApi` per page (or share one owned by the window), parented to
  a `QObject` that lives on the GUI thread.
- Capture `this` only when the page outlives the request; otherwise guard with
  `QPointer`.
- Do not issue overlapping *writes* through the same underlying `ApiClient`
  concurrently — token refresh on 401 mutates shared state. Serialize writes, or
  give independent flows their own client.
- The callback always runs on the GUI thread, so it is safe to touch widgets.

## Adoption status

`AsyncApi` is unit-tested (`tests/tst_asyncapi.cpp`) and ready. Pages should be
migrated incrementally, starting with the highest-traffic refresh paths
(`ItemsPage`, `DashboardPage`, `ReportsPage`, `TruckStockDashboardPage`).
