// AsyncApi.h - Non-blocking wrapper around IApiClient.
//
// The synchronous IApiClient calls block the calling thread for the duration
// of the HTTP request. Invoking them directly from the GUI thread freezes the
// UI (up to the request timeout) whenever the server is slow or unreachable.
//
// AsyncApi runs each call on a background thread (via Qt's global thread pool)
// and delivers the HttpResponse to a callback on the thread that owns this
// AsyncApi object -- normally the GUI thread -- so widgets can be updated
// safely without any manual thread marshalling.
//
// Usage:
//   asyncApi->get("/products", [this](const HttpResponse& res) {
//       // runs on the GUI thread; safe to touch widgets here
//       populateTable(res);
//   });
//
// Thread-safety note: a single IApiClient instance mutates its token state on
// 401 refresh, so avoid issuing overlapping writes through the same underlying
// client concurrently.
#pragma once

#include "HttpResponse.h"
#include "IApiClient.h"

#include <QObject>

#include <functional>
#include <string>

class AsyncApi : public QObject
{
    Q_OBJECT

public:
    using Callback = std::function<void(const HttpResponse&)>;

    explicit AsyncApi(IApiClient& api, QObject* parent = nullptr);

    void get(const std::string& endpoint, Callback callback);
    void post(const std::string& endpoint, const std::string& body, Callback callback);
    void put(const std::string& endpoint, const std::string& body, Callback callback);
    void patch(const std::string& endpoint, const std::string& body, Callback callback);
    void del(const std::string& endpoint, Callback callback);

private:
    void run(std::function<HttpResponse()> work, Callback callback);

    IApiClient& api_;
};
