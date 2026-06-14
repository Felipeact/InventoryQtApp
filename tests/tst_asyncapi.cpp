// Tests for AsyncApi: verifies that calls run off the caller's thread, do not
// block, and deliver their result back on the caller's thread.
#include <QtTest>
#include <QThread>

#include "AsyncApi.h"
#include "FakeApiClient.h"

class TestAsyncApi : public QObject
{
    Q_OBJECT

private slots:
    void get_deliversResultOnCallerThread()
    {
        FakeApiClient api;
        api.setResponse(200, R"({"ok":true})");
        AsyncApi async(api);

        HttpResponse received;
        bool done = false;
        QThread* callbackThread = nullptr;

        async.get("/things", [&](const HttpResponse& res) {
            received = res;
            callbackThread = QThread::currentThread();
            done = true;
        });

        QTRY_VERIFY_WITH_TIMEOUT(done, 5000);
        QCOMPARE(received.status_code, 200L);
        QCOMPARE(received.text, std::string(R"({"ok":true})"));
        // Callback must arrive on the thread that owns AsyncApi (the test/GUI
        // thread), not on the worker thread.
        QCOMPARE(callbackThread, QThread::currentThread());
        QCOMPARE(api.lastRequest().method, std::string("GET"));
        QCOMPARE(api.lastRequest().endpoint, std::string("/things"));
    }

    void get_returnsImmediatelyWithoutBlocking()
    {
        FakeApiClient api;
        AsyncApi async(api);

        bool done = false;
        async.get("/slow", [&](const HttpResponse&) { done = true; });

        // The call returns control right away; the callback has not run yet
        // because we have not pumped the event loop.
        QVERIFY(!done);

        QTRY_VERIFY_WITH_TIMEOUT(done, 5000);
    }

    void post_forwardsBody()
    {
        FakeApiClient api;
        api.setResponse(201, "{}");
        AsyncApi async(api);

        bool done = false;
        async.post("/things", R"({"name":"x"})", [&](const HttpResponse&) { done = true; });

        QTRY_VERIFY_WITH_TIMEOUT(done, 5000);
        QCOMPARE(api.lastRequest().method, std::string("POST"));
        QCOMPARE(api.lastRequest().body, std::string(R"({"name":"x"})"));
    }

    void del_issuesDeleteRequest()
    {
        FakeApiClient api;
        AsyncApi async(api);

        bool done = false;
        async.del("/things/7", [&](const HttpResponse&) { done = true; });

        QTRY_VERIFY_WITH_TIMEOUT(done, 5000);
        QCOMPARE(api.lastRequest().method, std::string("DELETE"));
        QCOMPARE(api.lastRequest().endpoint, std::string("/things/7"));
    }
};

QTEST_GUILESS_MAIN(TestAsyncApi)
#include "tst_asyncapi.moc"
