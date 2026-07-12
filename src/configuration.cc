#include "configuration.h"

#include <limits.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>

Configuration Configuration::Load(const std::string &path)
{
    Configuration configuration;

    if (!path.empty()) {
        std::ifstream ifs(path.c_str());
    
        if (!ifs) {
            configuration.state_ = MALFORMED;
            configuration.message_ = "Invalid configuration: bad path";
            return configuration;
        }

        std::ostringstream contents;
        contents << ifs.rdbuf();
        configuration = FromString(contents.str());
    } else {
        std::cout << "No configuration specified. Using default configuration.\n";
        configuration = Configuration::FromString("DocumentRoot .\nPort 8080\n");
    }

    configuration.canonicalize_document_root();

    return configuration;
}

Configuration Configuration::FromString(const std::string &text)
{
    Configuration configuration;

    std::size_t start, end;
    start = end = 0;

    // TODO: need more validation
    // Find DocumentRoot
    start = text.find("DocumentRoot");
    if (start == std::string::npos) {
        configuration.state_ = MALFORMED;
        configuration.message_ = "Invalid configuration: DocumentRoot not found";
        return configuration;
    }
    end = text.find("\n", start);
    start = text.find_first_of(" ", start);
    if ((start == std::string::npos) || (start > end)) {
        configuration.state_ = MALFORMED;
        configuration.message_ = "Invalid configuration: DocumentRoot bad format";
        return configuration;
    }
    configuration.document_root_ = text.substr(start + 1, end - start - 1);

    // TODO: need more validation
    // Find Port
    start = text.find("Port");
    if (start == std::string::npos) {
        configuration.state_ = MALFORMED;
        configuration.message_ = "Invalid configuration: Port not found";
        return configuration;
    }
    end = text.find("\n", start);
    start = text.find_first_of(" ", start);
    if ((start == std::string::npos) || (start > end)) {
        configuration.state_ = MALFORMED;
        configuration.message_ = "Invalid configuration: Port bad format";
        return configuration;
    }
    configuration.port_ = static_cast<unsigned short>(atoi(text.substr(start + 1, end - start - 1).c_str()));

    // Check for extra, unknown configuration
    if (text.size() == end + 1) {
        configuration.state_ = COMPLETE;
    } else {
        configuration.message_ = "Invalid configuration: Unknown keys";
        configuration.state_ = MALFORMED;
    }

    return configuration;
}

const std::string &Configuration::document_root() const
{
    return document_root_;
}

unsigned short Configuration::port() const
{
    return port_;
}

Configuration::State Configuration::state() const
{
    return state_;
}

const std::string &Configuration::message() const
{
    return message_;
}

void Configuration::canonicalize_document_root() {
    if (state_ != COMPLETE) return;

    char out[PATH_MAX];   
    if (realpath(document_root_.c_str(), out) == NULL) {
        state_ = MALFORMED;
        message_ = "Invalid configuration: DocumentRoot bad path";
        return;
    }
    document_root_ = out;
}
