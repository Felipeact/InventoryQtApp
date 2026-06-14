// Tests for CredentialStore. On non-Windows builds this exercises the
// obfuscation fallback and the legacy-plaintext migration path. (The Windows
// DPAPI path is covered by the same public contract on that platform.)
#include <QtTest>

#include "CredentialStore.h"

class TestCredentialStore : public QObject
{
    Q_OBJECT

private slots:
    void roundTripPreservesValue()
    {
        const QString secret = "eyJhbGciOiJIUzI1NiJ9.payload.signature";
        const QString stored = CredentialStore::protect(secret);
        QCOMPARE(CredentialStore::unprotect(stored), secret);
    }

    void protectDoesNotStorePlaintext()
    {
        const QString secret = "super-secret-token";
        const QString stored = CredentialStore::protect(secret);
        // The stored form must be tagged/encoded, not the raw token.
        QVERIFY(!stored.contains(secret));
        QVERIFY(stored.startsWith("DPAPI1:") || stored.startsWith("OBF1:"));
    }

    void emptyInputRoundTripsToEmpty()
    {
        QCOMPARE(CredentialStore::protect(""), QString());
        QCOMPARE(CredentialStore::unprotect(""), QString());
    }

    void legacyPlaintextIsReturnedUnchanged()
    {
        // Values written by older builds had no tag; they must still load.
        const QString legacy = "legacy-plaintext-token";
        QCOMPARE(CredentialStore::unprotect(legacy), legacy);
    }

    void unicodeSurvivesRoundTrip()
    {
        const QString secret = QString::fromUtf8("token-\xC3\xA9\xC3\xBC-\xE2\x9C\x93");
        QCOMPARE(CredentialStore::unprotect(CredentialStore::protect(secret)), secret);
    }
};

QTEST_GUILESS_MAIN(TestCredentialStore)
#include "tst_credentialstore.moc"
