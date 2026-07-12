#ifndef HTTPD98_WORKER_H
#define HTTPD98_WORKER_H

#include "configuration.h"

class Worker {
public:
    explicit Worker(const Configuration &configuration) : configuration_(configuration) { }

    void Execute(int conn);
private:
    const Configuration &configuration_;
};

#endif
