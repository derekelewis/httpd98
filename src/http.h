#ifndef HTTPD98_HTTP_H
#define HTTPD98_HTTP_H

#include <map>
#include <string>

namespace http {
    typedef std::map<std::string, std::string> Headers;
    typedef std::string Path;
    enum Method { UNKNOWN, OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT };
    enum StatusCode {
        HTTP_200 = 200,
        HTTP_400 = 400,
        HTTP_403 = 403,
        HTTP_404 = 404,
        HTTP_405 = 405,
        HTTP_500 = 500,
        HTTP_501 = 501,
        HTTP_505 = 505
    };
    std::string status_code_phrase(const StatusCode status_code);
    enum DecodeStatusCode {
        PENDING,
        COMPLETE,
        MALFORMED
    };
    DecodeStatusCode decode(const std::string &in, std::string &out);
    bool path_within(const std::string &base, const std::string &path);
}

#endif
