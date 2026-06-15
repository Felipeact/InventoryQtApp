#include "SecureStore.h"

#include <QByteArray>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <wincrypt.h>

#pragma comment(lib, "Crypt32.lib")

QString SecureStore::encrypt(const QString& plain)
{
    if (plain.isEmpty()) {
        return QString();
    }

    QByteArray input = plain.toUtf8();

    DATA_BLOB in;
    in.pbData = reinterpret_cast<BYTE*>(input.data());
    in.cbData = static_cast<DWORD>(input.size());

    DATA_BLOB out{};

    if (!CryptProtectData(
            &in,
            L"InventoryQtApp auth token",
            nullptr,
            nullptr,
            nullptr,
            CRYPTPROTECT_UI_FORBIDDEN,
            &out)) {
        return QString();
    }

    QByteArray encrypted(
        reinterpret_cast<const char*>(out.pbData),
        static_cast<int>(out.cbData)
    );

    LocalFree(out.pbData);

    return QString::fromLatin1(encrypted.toBase64());
}

QString SecureStore::decrypt(const QString& cipherBase64)
{
    if (cipherBase64.isEmpty()) {
        return QString();
    }

    QByteArray encrypted = QByteArray::fromBase64(cipherBase64.toLatin1());

    if (encrypted.isEmpty()) {
        return QString();
    }

    DATA_BLOB in;
    in.pbData = reinterpret_cast<BYTE*>(encrypted.data());
    in.cbData = static_cast<DWORD>(encrypted.size());

    DATA_BLOB out{};

    if (!CryptUnprotectData(
            &in,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            CRYPTPROTECT_UI_FORBIDDEN,
            &out)) {
        return QString();
    }

    QByteArray decrypted(
        reinterpret_cast<const char*>(out.pbData),
        static_cast<int>(out.cbData)
    );

    LocalFree(out.pbData);

    return QString::fromUtf8(decrypted);
}
