#include <iostream>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "request.h"
#include "request_parser.h"
#include "response.h"

int main(void) {

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == -1) {
        std::cerr << "socket failed: " << strerror(errno) << "\n";
        return 1;
    }

    int sockopt_val = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockopt_val, sizeof(sockopt_val)) == -1) {
        std::cerr << "setsockopt failed: " << strerror(errno) << "\n";
    }

    sockaddr_in sa;
    bzero(&sa, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(8080);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, reinterpret_cast<sockaddr *>(&sa), sizeof(sa)) != 0) {
        std::cerr << "bind failed: " << strerror(errno) << "\n";
        return 1;
    }

    if (listen(sock, 10) != 0) {
        std::cerr << "listen failed: " << strerror(errno) << "\n";
        return 1;
    }

    sockaddr_storage peer;
    socklen_t len = sizeof(peer);

    int conn;
    char buf[128];

    for(;;) {
        len = sizeof(peer);
        conn = accept(sock, reinterpret_cast<sockaddr *>(&peer), &len);
        if (conn == -1) {
            // TODO: do more handling here based on errno
            std::cerr << "accept failed: " << strerror(errno) << "\n";
            continue;
        }

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
            continue;
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

    return 0;
}
