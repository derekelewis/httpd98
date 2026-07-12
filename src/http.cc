#include "http.h"

std::string http::status_code_phrase(const StatusCode status_code)
{
    switch(status_code) {
        case HTTP_200: return "OK";
        case HTTP_400: return "Bad Request";
        case HTTP_403: return "Forbidden";
        case HTTP_404: return "Not Found";
        case HTTP_405: return "Method Not Allowed";
        case HTTP_500: return "Internal Server Error";
        case HTTP_501: return "Not Implemented";
        case HTTP_505: return "HTTP Version Not Supported";
    }
    return "Unknown";
}

static int from_hex(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

http::DecodeStatusCode http::decode(const std::string &in, std::string &out)
{
    out.clear();
    out.reserve(in.size());

    for (std::string::size_type i = 0; i < in.size(); ++i) {
        if (in[i] != '%') {
            out += in[i];
            continue;
        }
        // a '%' must be followed by two hex digits
        if (i + 2 >= in.size()) {
            return http::PENDING;
        }
        int high = from_hex(in[i + 1]);
        int low = from_hex(in[i + 2]);
        if (high == -1 || low == -1) {
            return http::MALFORMED;
        }
        char decoded = static_cast<char>(high * 16 + low);
        // an embedded NUL would silently truncate the path at the C string boundary
        if (decoded == '\0') {
            return http::MALFORMED;
        }
        out += decoded;
        i += 2;
    }

    return http::COMPLETE;
}

// base & path must be canonical paths (realpath)
bool http::path_within(const std::string &base, const std::string &path)
{
    // if base is /, then we ensure the path isn't empty and the first character of path is /
    // if path is /etc and base is /, then we correctly return true
    if (base == "/")
        return !path.empty() && path[0] == '/';

    // we ensure path matches base, and if not, we return false
    // we then test if path is the same size as base OR path is one character longer than base, but ends with '/'
    return path.compare(0, base.size(), base) == 0
        && (path.size() == base.size() || path[base.size()] == '/'); 
}
