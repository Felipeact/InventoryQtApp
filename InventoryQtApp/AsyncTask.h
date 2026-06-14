// AsyncTask.h - Run blocking work off the GUI thread, deliver the result back
// on the GUI thread.
//
// Most pages call the service layer synchronously; those calls block the
// calling thread for the duration of the HTTP request. AsyncTask::run executes
// the work on Qt's global thread pool and invokes the result callback on the
// thread that owns `context` (normally the GUI thread), so widgets can be
// updated directly and safely.
//
// It also supersedes the verbose, hand-rolled "QThread::create + QPointer +
// QMetaObject::invokeMethod" pattern found in some pages, and reuses a pooled
// thread instead of spawning a fresh OS thread per call.
//
// Lifetime: if `context` is destroyed before the work finishes, the result
// callback is not invoked (no use-after-free). The work itself still runs to
// completion on the pool, so it must capture only things that outlive the page
// (e.g. a service owned by the window), or copies.
//
// Usage:
//   AsyncTask::run(this,
//       [&svc = productService]() { return svc.getProducts(true); },
//       [this](json products) {            // runs on the GUI thread
//           currentProducts = products;
//           applyFilters();
//       });
#pragma once

#include <QFutureWatcher>
#include <QObject>
#include <QtConcurrent/QtConcurrentRun>

#include <type_traits>
#include <utility>

namespace AsyncTask
{
    template <typename Work, typename OnResult>
    void run(QObject* context, Work work, OnResult onResult)
    {
        using T = std::invoke_result_t<Work>;

        auto* watcher = new QFutureWatcher<T>(context);

        QObject::connect(watcher, &QFutureWatcher<T>::finished, context,
            [watcher, onResult = std::move(onResult)]() mutable {
                onResult(watcher->result());
                watcher->deleteLater();
            });

        watcher->setFuture(QtConcurrent::run(std::move(work)));
    }
}
