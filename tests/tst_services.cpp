// Tests for the service layer, exercised through the IApiClient seam with a
// FakeApiClient. These verify request shaping, status-code handling, response
// parsing, and resilience to malformed payloads -- with no network access.
#include <QtTest>

#include "FakeApiClient.h"
#include "AssetService.h"
#include "AuthService.h"
#include "ProductService.h"
#include "ReportService.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace
{
    bool startsWith(const std::string& s, const std::string& prefix)
    {
        return s.rfind(prefix, 0) == 0;
    }
}

class TestServices : public QObject
{
    Q_OBJECT

private slots:
    // --- AssetService -----------------------------------------------------
    void asset_create_postsToAssetsEndpoint()
    {
        FakeApiClient api;
        api.setResponse(201, "{}");
        AssetService svc(api);

        QVERIFY(svc.createAsset("Drill", "Tool", "SN1", "Active", "desc"));
        QCOMPARE(api.requestCount(), 1);
        QCOMPARE(api.lastRequest().method, std::string("POST"));
        QCOMPARE(api.lastRequest().endpoint, std::string("/assets"));

        json sent = json::parse(api.lastRequest().body);
        QCOMPARE(sent["name"], "Drill");
        QCOMPARE(sent["serialCode"], "SN1");
    }

    void asset_create_failsOnErrorStatus()
    {
        FakeApiClient api;
        api.setResponse(400, "bad");
        AssetService svc(api);
        QVERIFY(!svc.createAsset("Drill", "Tool", "SN1", "Active"));
    }

    void asset_getAssets_normalizesEnvelope()
    {
        FakeApiClient api;
        api.setResponse(200, R"({"data":[{"name":"A"},{"name":"B"}]})");
        AssetService svc(api);

        json assets = svc.getAssets();
        QCOMPARE(assets.size(), size_t(2));
        QCOMPARE(assets[0]["status"], "Active"); // normalization applied
    }

    void asset_getAssets_emptyOnNon200()
    {
        FakeApiClient api;
        api.setResponse(500, "boom");
        AssetService svc(api);
        QCOMPARE(svc.getAssets().size(), size_t(0));
    }

    void asset_getAssets_emptyOnMalformedJson()
    {
        FakeApiClient api;
        api.setResponse(200, "not json {");
        AssetService svc(api);
        QCOMPARE(svc.getAssets().size(), size_t(0));
    }

    void asset_update_usesPutWithId()
    {
        FakeApiClient api;
        api.setResponse(200, "{}");
        AssetService svc(api);

        QVERIFY(svc.updateAsset("abc", "n", "t", "s", "Active"));
        QCOMPARE(api.lastRequest().method, std::string("PUT"));
        QCOMPARE(api.lastRequest().endpoint, std::string("/assets/abc"));
    }

    void asset_delete_acceptsBoth200And204()
    {
        FakeApiClient api;
        AssetService svc(api);

        api.setResponse(200, "");
        QVERIFY(svc.deleteAsset("x"));
        api.setResponse(204, "");
        QVERIFY(svc.deleteAsset("x"));
        api.setResponse(404, "");
        QVERIFY(!svc.deleteAsset("x"));
        QCOMPARE(api.lastRequest().method, std::string("DELETE"));
    }

    void asset_search_encodesQueryValue()
    {
        FakeApiClient api;
        api.setResponse(200, "[]");
        AssetService svc(api);

        svc.searchAssets("a b&c");
        const std::string ep = api.lastRequest().endpoint;
        QVERIFY2(ep.find("search=a%20b%26c") != std::string::npos, ep.c_str());
        // The injected '&' must not appear raw (which would split the query).
        QVERIFY(ep.find("&c") == std::string::npos);
    }

    // --- AuthService ------------------------------------------------------
    void auth_login_succeedsAndExtractsName()
    {
        FakeApiClient api;
        api.setResponse(200, R"({"accessToken":"a","refreshToken":"r","user":{"name":"Alice"}})");
        AuthService svc(api);

        LoginResult res = svc.login("a@x.com", "pw");
        QVERIFY(res.success);
        QCOMPARE(res.accessToken, std::string("a"));
        QCOMPARE(res.refreshToken, std::string("r"));
        QCOMPARE(res.userName, std::string("Alice"));
        QCOMPARE(api.lastRequest().endpoint, std::string("/auth/login"));
    }

    void auth_login_failsOnMissingTokens()
    {
        FakeApiClient api;
        api.setResponse(200, R"({"user":{"name":"Alice"}})");
        AuthService svc(api);

        LoginResult res = svc.login("a@x.com", "pw");
        QVERIFY(!res.success);
        QVERIFY(!res.errorMessage.empty());
    }

    void auth_login_failsOnNon200()
    {
        FakeApiClient api;
        api.setResponse(401, "Invalid credentials");
        AuthService svc(api);

        LoginResult res = svc.login("a@x.com", "pw");
        QVERIFY(!res.success);
        QCOMPARE(res.errorMessage, std::string("Invalid credentials"));
    }

    void auth_login_failsGracefullyOnMalformedJson()
    {
        FakeApiClient api;
        api.setResponse(200, "}{ not json");
        AuthService svc(api);

        LoginResult res = svc.login("a@x.com", "pw");
        QVERIFY(!res.success);
    }

    void auth_logout_withoutRefreshTokenClearsLocally()
    {
        FakeApiClient api;
        AuthService svc(api);

        QVERIFY(svc.logout());
        QVERIFY(api.tokensCleared);
        QCOMPARE(api.requestCount(), 0); // no network call when no refresh token
    }

    void auth_logout_withRefreshTokenCallsEndpoint()
    {
        FakeApiClient api;
        api.setRefreshToken("r");
        api.setResponse(200, "{}");
        AuthService svc(api);

        QVERIFY(svc.logout());
        QCOMPARE(api.lastRequest().endpoint, std::string("/auth/logout"));
        QVERIFY(api.tokensCleared);
    }

    // --- ProductService ---------------------------------------------------
    void product_create_postsToProductsEndpoint()
    {
        FakeApiClient api;
        api.setResponse(201, "{}");
        ProductService svc(api);

        QVERIFY(svc.createProduct("Bolt", "BC1", 10));
        QCOMPARE(api.lastRequest().method, std::string("POST"));
        QCOMPARE(api.lastRequest().endpoint, std::string("/products"));
        json sent = json::parse(api.lastRequest().body);
        QCOMPARE(sent["quantity"], 10);
    }

    void product_getProducts_returnsDataArray()
    {
        FakeApiClient api;
        api.setResponse(200, R"({"data":[{"name":"P"}]})");
        ProductService svc(api);

        json products = svc.getProducts();
        QCOMPARE(products.size(), size_t(1));
        QVERIFY(startsWith(api.lastRequest().endpoint, "/products"));
    }

    void product_getNameByBarcode_matchesExact()
    {
        FakeApiClient api;
        api.setResponse(200, R"({"data":[{"barcode":"BC1","name":"Bolt"}]})");
        ProductService svc(api);
        QCOMPARE(svc.getProductNameByBarcode("BC1"), std::string("Bolt"));
    }

    void product_getNameByBarcode_returnsBarcodeWhenNoMatch()
    {
        FakeApiClient api;
        api.setResponse(200, R"({"data":[]})");
        ProductService svc(api);
        QCOMPARE(svc.getProductNameByBarcode("NOPE"), std::string("NOPE"));
    }

    void product_scanIn_postsToScanInEndpoint()
    {
        FakeApiClient api;
        api.setResponse(200, "{}");
        ProductService svc(api);

        QVERIFY(svc.scanIn("BC1", 3));
        QCOMPARE(api.lastRequest().endpoint, std::string("/products/scan-in"));
    }

    // --- ReportService (regression: must not throw on bad payload) --------
    void report_inventorySummary_emptyObjectOnMalformedJson()
    {
        FakeApiClient api;
        api.setResponse(200, "<<not json>>");
        ReportService svc(api);

        json summary = svc.getInventorySummary(); // previously threw -> crash
        QVERIFY(summary.is_object());
        QCOMPARE(summary.size(), size_t(0));
    }

    void report_inventorySummary_emptyObjectOnNon200()
    {
        FakeApiClient api;
        api.setResponse(503, "down");
        ReportService svc(api);
        QVERIFY(svc.getInventorySummary().is_object());
    }

    void report_inventorySummary_parsesValidPayload()
    {
        FakeApiClient api;
        api.setResponse(200, R"({"total":5})");
        ReportService svc(api);
        QCOMPARE(svc.getInventorySummary()["total"], 5);
    }
};

QTEST_APPLESS_MAIN(TestServices)
#include "tst_services.moc"
