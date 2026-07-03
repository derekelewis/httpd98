#include "request.h"
#include "request_parser.h"
#include "response.h"
#include "worker.h"

#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <string.h>

void Worker::Execute(int conn) {
    char buf[128];

    std::cout << "connection accepted\n";
    std::string request_string = "";
    ssize_t bytes_read = 0;
    while((bytes_read = read(conn, buf, sizeof(buf))) > 0) {
        request_string.append(buf, bytes_read);
        if (request_string.find("\r\n\r\n") != std::string::npos) break;
    }
    RequestParser request_parser;
    Request request = request_parser.ParseRequest(request_string);
    if (request.state() == Request::MALFORMED) {
        // TODO: return 400 response
        close(conn);
        return;
    }
    // TODO: return actual response
    switch (request.method()) {
        case http::GET: {
            std::string body = "<html><body>hello, world!</body></html>";
            Response response = Response(http::HTTP_200, body);
            std::string response_string = response.serialize();
            write(conn, response_string.data(), response_string.size());
            close(conn);
            break;
        }
        default:
            // unimplemented
            close(conn);
    }
}


