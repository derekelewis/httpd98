#ifndef HTTPD98_STATIC_FILE_HANDLER_H
#define HTTPD98_STATIC_FILE_HANDLER_H

#include <string>

#include "handler.h"

class StaticFileHandler : public Handler {
public:
    explicit StaticFileHandler(const std::string &document_root) : document_root_(document_root) {}
    virtual Response Handle(const Request &request) const;
private:
    const std::string document_root_;
};

#endif