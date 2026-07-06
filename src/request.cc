#include "request.h"

http::Method Request::method() const
{
    return method_;
}

const http::Path &Request::path() const
{
    return path_;
}

Request::State Request::state() const
{
    return state_;
}

const http::Headers &Request::headers() const
{
    return headers_;
}

// TODO: implement a companion has_header since we return an empty value for both: (1) headers that don't exist and (2) do exist, but are empty

// since we don't have std::map::at in C++98
const std::string &Request::header(const std::string &header) const {
    http::Headers::const_iterator it = headers_.find(header);
    if (it == headers_.end()) {
        static const std::string empty;
        return empty;
    }
    return it->second;
}
