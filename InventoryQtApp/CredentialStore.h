// CredentialStore.h - Encrypts sensitive values (auth tokens) before they are
// written to persistent storage.
//
// On Windows the platform DPAPI (CryptProtectData) is used, binding the
// ciphertext to the current user account. On other platforms a machine-keyed
// obfuscation is applied as a best-effort fallback. Values produced by
// protect() are self-describing (tagged), and unprotect() transparently
// migrates legacy plaintext values written by older builds.
#pragma once

#include <QString>

namespace CredentialStore
{
    // Returns a tagged, encoded representation safe to persist. Empty input
    // yields an empty string.
    QString protect(const QString& plaintext);

    // Reverses protect(). Untagged input is treated as legacy plaintext and
    // returned unchanged so existing saved sessions keep working.
    QString unprotect(const QString& stored);
}
