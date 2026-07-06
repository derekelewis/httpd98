#ifndef HTTPD98_REQUEST_H
#define HTTPD98_REQUEST_H
#include <string>

#include "http.h"

class Request {
public:
    enum State { PENDING, COMPLETE, INCOMPLETE, MALFORMED };
    Request() : method_(http::UNKNOWN), state_(PENDING) {}
    http::Method method() const;
    http::Path path() const;
    State state() const;
    const http::Headers &headers() const;
    const std::string &header(const std::string &header) const;
private:
    friend class RequestParser;
    http::Method method_;
    http::Path path_;
    State state_;
    http::Headers headers_;
};

#endif
