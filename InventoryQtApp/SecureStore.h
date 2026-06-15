// SecureStore.h - At-rest encryption for sensitive values (auth tokens).
//
// Uses the Windows Data Protection API (DPAPI) scoped to the current user, so
// stored secrets cannot be read by other users or copied to another machine.
// Values are returned base64-encoded for safe storage in QSettings.
#pragma once

#include <QString>

namespace SecureStore
{
    // Encrypts a string with DPAPI (current-user scope) and returns it base64
    // encoded. Returns an empty string for empty input or on failure.
    QString encrypt(const QString& plain);

    // Reverses encrypt(). Returns an empty string if the input was not produced
    // by encrypt() on this user account/machine (e.g. legacy plaintext data).
    QString decrypt(const QString& cipherBase64);
}
