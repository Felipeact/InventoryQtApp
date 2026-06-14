// Tests for JsonUtils: safe field access, asset normalization, and user-name
// resolution.
#include <QtTest>

#include "JsonUtils.h"

using json = nlohmann::json;

class TestJsonUtils : public QObject
{
    Q_OBJECT

private slots:
    // --- safeString -------------------------------------------------------
    void safeString_returnsValueWhenPresent()
    {
        json j = { {"name", "Hammer"} };
        QCOMPARE(JsonUtils::safeString(j, "name"), std::string("Hammer"));
    }

    void safeString_usesFallbackWhenMissing()
    {
        json j = json::object();
        QCOMPARE(JsonUtils::safeString(j, "name", "n/a"), std::string("n/a"));
    }

    void safeString_usesFallbackWhenNull()
    {
        json j = { {"name", nullptr} };
        QCOMPARE(JsonUtils::safeString(j, "name", "n/a"), std::string("n/a"));
    }

    void safeString_usesFallbackWhenNotString()
    {
        json j = { {"name", 42} };
        QCOMPARE(JsonUtils::safeString(j, "name", "n/a"), std::string("n/a"));
    }

    void safeString_handlesNonObjectInput()
    {
        json arr = json::array();
        QCOMPARE(JsonUtils::safeString(arr, "name", "n/a"), std::string("n/a"));
    }

    // --- normalizeAsset ---------------------------------------------------
    void normalizeAsset_fillsMissingFields()
    {
        json asset = JsonUtils::normalizeAsset(json::object());
        QCOMPARE(asset["id"], "");
        QCOMPARE(asset["name"], "");
        // status defaults to "Active"
        QCOMPARE(asset["status"], "Active");
    }

    void normalizeAsset_preservesProvidedStatus()
    {
        json asset = JsonUtils::normalizeAsset(json{ {"status", "Retired"} });
        QCOMPARE(asset["status"], "Retired");
    }

    // --- normalizeAssetsResponse -----------------------------------------
    void normalizeAssetsResponse_handlesDataEnvelope()
    {
        json response = { {"data", json::array({ json{{"name", "A"}}, json{{"name", "B"}} })} };
        json out = JsonUtils::normalizeAssetsResponse(response);
        QCOMPARE(out.size(), size_t(2));
        QCOMPARE(out[0]["name"], "A");
        QCOMPARE(out[0]["status"], "Active");
    }

    void normalizeAssetsResponse_handlesBareArray()
    {
        json response = json::array({ json{{"name", "A"}} });
        json out = JsonUtils::normalizeAssetsResponse(response);
        QCOMPARE(out.size(), size_t(1));
    }

    void normalizeAssetsResponse_skipsNonObjects()
    {
        json response = json::array({ json{{"name", "A"}}, json("string"), json(5) });
        json out = JsonUtils::normalizeAssetsResponse(response);
        QCOMPARE(out.size(), size_t(1));
    }

    void normalizeAssetsResponse_emptyForUnexpectedShape()
    {
        QCOMPARE(JsonUtils::normalizeAssetsResponse(json(42)).size(), size_t(0));
        QCOMPARE(JsonUtils::normalizeAssetsResponse(json::object()).size(), size_t(0));
    }

    // --- readUserNameFromJson --------------------------------------------
    void readUserName_prefersDirectName()
    {
        json u = { {"name", "Alice"}, {"email", "a@x.com"} };
        QCOMPARE(JsonUtils::readUserNameFromJson(u, "fb@x.com"), std::string("Alice"));
    }

    void readUserName_fallsBackThroughAlternateKeys()
    {
        json u = { {"displayName", "Bob"} };
        QCOMPARE(JsonUtils::readUserNameFromJson(u, "fb@x.com"), std::string("Bob"));
    }

    void readUserName_combinesFirstAndLast()
    {
        json u = { {"firstName", "Jane"}, {"lastName", "Doe"} };
        QCOMPARE(JsonUtils::readUserNameFromJson(u, "fb@x.com"), std::string("Jane Doe"));
    }

    void readUserName_usesFirstNameOnly()
    {
        json u = { {"firstName", "Jane"} };
        QCOMPARE(JsonUtils::readUserNameFromJson(u, "fb@x.com"), std::string("Jane"));
    }

    void readUserName_fallsBackToEmail()
    {
        json u = { {"email", "user@x.com"} };
        QCOMPARE(JsonUtils::readUserNameFromJson(u, "fb@x.com"), std::string("user@x.com"));
    }

    void readUserName_fallsBackToProvidedEmail()
    {
        QCOMPARE(JsonUtils::readUserNameFromJson(json::object(), "fb@x.com"), std::string("fb@x.com"));
        QCOMPARE(JsonUtils::readUserNameFromJson(json(7), "fb@x.com"), std::string("fb@x.com"));
    }

    void readUserName_ignoresEmptyDirectName()
    {
        json u = { {"name", ""}, {"email", "user@x.com"} };
        QCOMPARE(JsonUtils::readUserNameFromJson(u, "fb@x.com"), std::string("user@x.com"));
    }
};

QTEST_APPLESS_MAIN(TestJsonUtils)
#include "tst_jsonutils.moc"
