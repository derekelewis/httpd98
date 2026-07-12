#ifndef HTTPD98_CONFIGURATION_H
#define HTTPD98_CONFIGURATION_H

#include <string>

class Configuration {
public:
    enum State { PENDING, COMPLETE, MALFORMED };

    static Configuration Load(const std::string &path);
    static Configuration FromString(const std::string &text);

    const std::string &document_root() const;
    unsigned short port() const;
    State state() const;
    const std::string &message() const;

private:
    Configuration() : document_root_(""), port_(8080), state_(PENDING) {}

    std::string document_root_;
    unsigned short port_; 
    State state_;
    std::string message_;

    void canonicalize_document_root();
};

#endif
