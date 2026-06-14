// Tests for ErrorHandler: HTTP status-code -> user message mapping and the
// network/auth/availability classifiers.
#include <QtTest>

#include "ErrorHandler.h"
#include "HttpResponse.h"

namespace
{
    HttpResponse resp(long status, const std::string& text = "")
    {
        return HttpResponse{ status, text };
    }
}

class TestErrorHandler : public QObject
{
    Q_OBJECT

private slots:
    void networkError_isReportedAsConnectionProblem()
    {
        const auto msg = ErrorHandler::getErrorMessage(resp(0));
        QVERIFY(msg.find("Unable to connect") != std::string::npos);
    }

    void knownStatusCodes_haveSpecificMessages_data()
    {
        QTest::addColumn<int>("status");
        QTest::addColumn<QString>("needle");

        QTest::newRow("401") << 401 << "session has expired";
        QTest::newRow("403") << 403 << "do not have permission";
        QTest::newRow("404") << 404 << "Not Found";
        QTest::newRow("429") << 429 << "Too Many Requests";
        QTest::newRow("500") << 500 << "internal server error";
        QTest::newRow("503") << 503 << "maintenance";
    }

    void knownStatusCodes_haveSpecificMessages()
    {
        QFETCH(int, status);
        QFETCH(QString, needle);

        const auto msg = QString::fromStdString(ErrorHandler::getErrorMessage(resp(status)));
        QVERIFY2(msg.contains(needle), qPrintable("status " + QString::number(status) + " -> " + msg));
    }

    void successCodes_returnSuccess()
    {
        QCOMPARE(ErrorHandler::getErrorMessage(resp(200)), std::string("Success"));
        QCOMPARE(ErrorHandler::getErrorMessage(resp(201)), std::string("Success"));
        QCOMPARE(ErrorHandler::getErrorMessage(resp(204)), std::string("Success"));
    }

    void unknownServerError_includesCode()
    {
        const auto msg = ErrorHandler::getErrorMessage(resp(599));
        QVERIFY(msg.find("599") != std::string::npos);
        QVERIFY(msg.find("Server Error") != std::string::npos);
    }

    void unknownClientError_includesCode()
    {
        const auto msg = ErrorHandler::getErrorMessage(resp(418));
        QVERIFY(msg.find("418") != std::string::npos);
        QVERIFY(msg.find("Request Error") != std::string::npos);
    }

    void isNetworkError_onlyForZero()
    {
        QVERIFY(ErrorHandler::isNetworkError(resp(0)));
        QVERIFY(!ErrorHandler::isNetworkError(resp(500)));
    }

    void isServerUnavailable_coversGatewayCodes()
    {
        QVERIFY(ErrorHandler::isServerUnavailable(resp(0)));
        QVERIFY(ErrorHandler::isServerUnavailable(resp(502)));
        QVERIFY(ErrorHandler::isServerUnavailable(resp(503)));
        QVERIFY(ErrorHandler::isServerUnavailable(resp(504)));
        QVERIFY(!ErrorHandler::isServerUnavailable(resp(500)));
        QVERIFY(!ErrorHandler::isServerUnavailable(resp(200)));
    }

    void isAuthenticationError_for401And403()
    {
        QVERIFY(ErrorHandler::isAuthenticationError(resp(401)));
        QVERIFY(ErrorHandler::isAuthenticationError(resp(403)));
        QVERIFY(!ErrorHandler::isAuthenticationError(resp(404)));
    }

    void detailedInfo_includesTroubleshootingOnlyForNetworkErrors()
    {
        const auto net = ErrorHandler::getDetailedErrorInfo(resp(0));
        QVERIFY(net.find("Troubleshooting") != std::string::npos);

        const auto server = ErrorHandler::getDetailedErrorInfo(resp(500, "boom"));
        QVERIFY(server.find("Troubleshooting") == std::string::npos);
        QVERIFY(server.find("boom") != std::string::npos);
    }
};

QTEST_APPLESS_MAIN(TestErrorHandler)
#include "tst_errorhandler.moc"
