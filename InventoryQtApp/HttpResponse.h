// HttpResponse.h - Transport-agnostic HTTP response.
//
// This is a lightweight value type that decouples the application from any
// concrete HTTP library (currently cpr). Only the fields the application
// actually consumes are exposed, which keeps the rest of the codebase free of
// third-party HTTP headers and makes the network layer trivially mockable in
// tests.
#pragma once

#include <string>

struct HttpResponse
{
    // HTTP status code. 0 means the request never reached the server
    // (network error, connection refused, timeout, etc.).
    long status_code = 0;

    // Raw response body.
    std::string text;
};
