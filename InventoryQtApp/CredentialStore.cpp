#include "CredentialStore.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QSysInfo>

#ifdef _WIN32
#include <windows.h>
#include <dpapi.h>
#pragma comment(lib, "crypt32.lib")
#endif

namespace
{
    const char* kDpapiTag = "DPAPI1:";
    const char* kObfTag = "OBF1:";

    // Derives a process-stable key bound to the machine where possible. Used
    // only for the non-DPAPI obfuscation fallback.
    QByteArray deriveKey()
    {
        QByteArray seed = QByteArrayLiteral("InventoryQtApp/v1/credential");

        const QByteArray machine = QSysInfo::machineUniqueId();
        if (!machine.isEmpty()) {
            seed += machine;
        }

        return QCryptographicHash::hash(seed, QCryptographicHash::Sha256);
    }

    QByteArray xorWithKey(const QByteArray& data)
    {
        const QByteArray key = deriveKey();
        QByteArray out = data;
        for (int i = 0; i < out.size(); ++i) {
            out[i] = out[i] ^ key[i % key.size()];
        }
        return out;
    }
}

QString CredentialStore::protect(const QString& plaintext)
{
    if (plaintext.isEmpty()) {
        return QString();
    }

    const QByteArray data = plaintext.toUtf8();

#ifdef _WIN32
    DATA_BLOB in;
    in.cbData = static_cast<DWORD>(data.size());
    in.pbData = reinterpret_cast<BYTE*>(const_cast<char*>(data.constData()));

    DATA_BLOB out{};
    if (CryptProtectData(&in, L"InventoryQtApp", nullptr, nullptr, nullptr, 0, &out)) {
        const QByteArray blob(reinterpret_cast<const char*>(out.pbData), static_cast<int>(out.cbData));
        LocalFree(out.pbData);
        return QString::fromLatin1(kDpapiTag) + QString::fromLatin1(blob.toBase64());
    }
    // If DPAPI fails for any reason, fall through to the obfuscation fallback.
#endif

    return QString::fromLatin1(kObfTag) + QString::fromLatin1(xorWithKey(data).toBase64());
}

QString CredentialStore::unprotect(const QString& stored)
{
    if (stored.isEmpty()) {
        return QString();
    }

    if (stored.startsWith(QLatin1String(kDpapiTag))) {
#ifdef _WIN32
        QByteArray blob = QByteArray::fromBase64(stored.mid(static_cast<int>(qstrlen(kDpapiTag))).toLatin1());

        DATA_BLOB in;
        in.cbData = static_cast<DWORD>(blob.size());
        in.pbData = reinterpret_cast<BYTE*>(blob.data());

        DATA_BLOB out{};
        if (CryptUnprotectData(&in, nullptr, nullptr, nullptr, nullptr, 0, &out)) {
            const QByteArray plain(reinterpret_cast<const char*>(out.pbData), static_cast<int>(out.cbData));
            LocalFree(out.pbData);
            return QString::fromUtf8(plain);
        }
#endif
        // Cannot decrypt (different user/machine, or non-Windows build reading
        // a Windows-encrypted value): treat as unavailable.
        return QString();
    }

    if (stored.startsWith(QLatin1String(kObfTag))) {
        const QByteArray data = QByteArray::fromBase64(stored.mid(static_cast<int>(qstrlen(kObfTag))).toLatin1());
        return QString::fromUtf8(xorWithKey(data));
    }

    // Legacy plaintext written before encryption was introduced. Return as-is
    // so the session is preserved; it will be re-protected on next save.
    return stored;
}
