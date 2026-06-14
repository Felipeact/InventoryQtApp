// Tests for AsyncTask::run -- the generic off-thread / deliver-on-caller-thread
// helper used to make service calls non-blocking.
#include <QtTest>
#include <QObject>
#include <QThread>

#include "AsyncTask.h"

class TestAsyncTask : public QObject
{
    Q_OBJECT

private slots:
    void deliversResultOnCallerThread()
    {
        int result = 0;
        bool done = false;
        QThread* callbackThread = nullptr;

        AsyncTask::run(this,
            []() { return 21 * 2; },
            [&](int value) {
                result = value;
                callbackThread = QThread::currentThread();
                done = true;
            });

        QTRY_VERIFY_WITH_TIMEOUT(done, 5000);
        QCOMPARE(result, 42);
        QCOMPARE(callbackThread, QThread::currentThread());
    }

    void workRunsOnADifferentThread()
    {
        QThread* workThread = nullptr;
        bool done = false;

        AsyncTask::run(this,
            [&]() { workThread = QThread::currentThread(); return 1; },
            [&](int) { done = true; });

        QTRY_VERIFY_WITH_TIMEOUT(done, 5000);
        QVERIFY(workThread != nullptr);
        QVERIFY(workThread != QThread::currentThread());
    }

    void doesNotBlockCaller()
    {
        bool done = false;
        AsyncTask::run(this, []() { return 1; }, [&](int) { done = true; });
        // Callback has not run because the event loop has not been pumped yet.
        QVERIFY(!done);
        QTRY_VERIFY_WITH_TIMEOUT(done, 5000);
    }

    void supportsNonTrivialReturnTypes()
    {
        QString out;
        bool done = false;

        AsyncTask::run(this,
            []() { return QString("hello"); },
            [&](QString value) { out = value; done = true; });

        QTRY_VERIFY_WITH_TIMEOUT(done, 5000);
        QCOMPARE(out, QString("hello"));
    }

    void callbackSkippedWhenContextDestroyedEarly()
    {
        bool called = false;
        {
            auto* ctx = new QObject;
            AsyncTask::run(ctx,
                []() { QThread::msleep(50); return 1; },
                [&](int) { called = true; });
            delete ctx; // destroyed before the work finishes
        }
        // Give the pool time to finish; the callback must not fire.
        QTest::qWait(200);
        QVERIFY(!called);
    }
};

QTEST_GUILESS_MAIN(TestAsyncTask)
#include "tst_asynctask.moc"
