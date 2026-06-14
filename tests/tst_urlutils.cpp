// Tests for UrlUtils::encodeQueryValue percent-encoding.
#include <QtTest>

#include "UrlUtils.h"

class TestUrlUtils : public QObject
{
    Q_OBJECT

private slots:
    void unreservedCharactersPassThrough()
    {
        QCOMPARE(UrlUtils::encodeQueryValue("Abc-123_x.y~z"), std::string("Abc-123_x.y~z"));
    }

    void spacesAreEncoded()
    {
        QCOMPARE(UrlUtils::encodeQueryValue("hello world"), std::string("hello%20world"));
    }

    void reservedCharactersAreEncoded()
    {
        QCOMPARE(UrlUtils::encodeQueryValue("a&b=c?d/e"), std::string("a%26b%3Dc%3Fd%2Fe"));
    }

    void ampersandCannotBreakQuery()
    {
        // The whole point: an injected '&' must not start a new parameter.
        const std::string encoded = UrlUtils::encodeQueryValue("x&admin=true");
        QVERIFY(encoded.find('&') == std::string::npos);
    }

    void emptyStringStaysEmpty()
    {
        QCOMPARE(UrlUtils::encodeQueryValue(""), std::string(""));
    }

    void encodesUppercaseHex()
    {
        // 0x20 (space) must be %20, not %2g or lowercase.
        QCOMPARE(UrlUtils::encodeQueryValue(" "), std::string("%20"));
    }
};

QTEST_APPLESS_MAIN(TestUrlUtils)
#include "tst_urlutils.moc"
