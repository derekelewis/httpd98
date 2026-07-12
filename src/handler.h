#ifndef HTTPD98_HANDLER_H
#define HTTPD98_HANDLER_H

#include "request.h"
#include "response.h"

class Handler {
public:
    virtual ~Handler() {};
    
    virtual Response Handle(const Request &request) const = 0;
};

#endif