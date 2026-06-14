// UrlUtils.h - Minimal RFC 3986 percent-encoding helpers.
//
// Used to safely place user-supplied text (search terms, ids) into URL query
// strings. Without this, values containing spaces or reserved characters
// produce malformed requests.
#pragma once

#include <string>

namespace UrlUtils
{
    // Percent-encodes a value for use inside a URL query component. Unreserved
    // characters (RFC 3986 section 2.3) are passed through unchanged; every
    // other byte is encoded as %HH.
    inline std::string encodeQueryValue(const std::string& value)
    {
        static const char* hex = "0123456789ABCDEF";
        std::string out;
        out.reserve(value.size() * 3);

        for (unsigned char c : value) {
            const bool unreserved =
                (c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                (c >= '0' && c <= '9') ||
                c == '-' || c == '_' || c == '.' || c == '~';

            if (unreserved) {
                out.push_back(static_cast<char>(c));
            }
            else {
                out.push_back('%');
                out.push_back(hex[(c >> 4) & 0xF]);
                out.push_back(hex[c & 0xF]);
            }
        }

        return out;
    }
}
