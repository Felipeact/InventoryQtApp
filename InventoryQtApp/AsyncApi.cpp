#include "AsyncApi.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

AsyncApi::AsyncApi(IApiClient& api, QObject* parent)
    : QObject(parent)
    , api_(api)
{
}

void AsyncApi::get(const std::string& endpoint, Callback callback)
{
    run([this, endpoint]() { return api_.get(endpoint); }, std::move(callback));
}

void AsyncApi::post(const std::string& endpoint, const std::string& body, Callback callback)
{
    run([this, endpoint, body]() { return api_.post(endpoint, body); }, std::move(callback));
}

void AsyncApi::put(const std::string& endpoint, const std::string& body, Callback callback)
{
    run([this, endpoint, body]() { return api_.put(endpoint, body); }, std::move(callback));
}

void AsyncApi::patch(const std::string& endpoint, const std::string& body, Callback callback)
{
    run([this, endpoint, body]() { return api_.patch(endpoint, body); }, std::move(callback));
}

void AsyncApi::del(const std::string& endpoint, Callback callback)
{
    run([this, endpoint]() { return api_.del(endpoint); }, std::move(callback));
}

void AsyncApi::run(std::function<HttpResponse()> work, Callback callback)
{
    // The watcher is parented to this object, so its finished() signal is
    // delivered on this object's thread (the GUI thread), which is where the
    // callback then runs.
    auto* watcher = new QFutureWatcher<HttpResponse>(this);

    connect(watcher, &QFutureWatcher<HttpResponse>::finished, this,
        [watcher, cb = std::move(callback)]() {
            if (cb) {
                cb(watcher->result());
            }
            watcher->deleteLater();
        });

    watcher->setFuture(QtConcurrent::run(std::move(work)));
}
