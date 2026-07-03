#include <iostream>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "request.h"
#include "request_parser.h"
#include "response.h"
#include "worker.h"

#define BACKLOG 4096

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

    if (listen(sock, BACKLOG) != 0) {
        std::cerr << "listen failed: " << strerror(errno) << "\n";
        return 1;
    }

    sockaddr_storage peer;
    socklen_t len = sizeof(peer);

    int conn;
    pid_t pid;

    // TODO: implement real signal handlers
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    for(;;) {
        len = sizeof(peer);
        conn = accept(sock, reinterpret_cast<sockaddr *>(&peer), &len);

        if (conn == -1) {
            // TODO: do more handling here based on errno
            std::cerr << "accept failed: " << strerror(errno) << "\n";
            continue;
        }

        if ((pid = fork()) == 0) {
            close(sock);
            Worker worker;
            worker.Execute(conn);
            // we assume Execute() handles all closes of conn before returning
            _exit(0);
        }

        if (pid == -1) {
            // TODO: do more error handling here; generate response?
            std::cerr << "fork failed: " << strerror(errno) << "\n";
        }

        close(conn);
    }

    return 0;
}
