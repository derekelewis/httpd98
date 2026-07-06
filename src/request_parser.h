#ifndef HTTPD98_REQUEST_PARSER_H
#define HTTPD98_REQUEST_PARSER_H
#include "request.h"

class RequestParser {
public:
    Request ParseRequest(const std::string &request) const;
};

#endif
