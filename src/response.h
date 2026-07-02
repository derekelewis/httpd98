#ifndef HTTPD98_RESPONSE_H
#define HTTPD98_RESPONSE_H

#include <string>

#include "http.h"

class Response {
public:
    Response(http::StatusCode status_code, const std::string& body) : status_code_(status_code), body_(body) {}
    std::string serialize() const;
private:
    http::Headers headers_;
    http::StatusCode status_code_;
    std::string body_;
};

#endif
