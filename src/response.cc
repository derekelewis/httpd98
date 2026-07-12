#include "response.h"

#include <ctime>
#include <sstream>

void Response::set_header(const std::string &name, const std::string &value)
{
    headers_[name] = value;
}

std::string Response::serialize() const
{
    std::ostringstream response;

    response << "HTTP/1.1 " << status_code_ << " " << http::status_code_phrase(status_code_) << "\r\n";
    // RFC 7231 requires the Date header in IMF-fixdate format, always in GMT
    char date[64] = "";
    std::time_t now = std::time(NULL);
    std::tm gmt;
    if (gmtime_r(&now, &gmt) != NULL
        && std::strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", &gmt) > 0) {
        response << "Date: " << date << "\r\n";
    }
    if (headers_.find("Content-Type") == headers_.end()) {
        response << "Content-Type: text/html" << "\r\n";
    }
    response << "Content-Length: " << body_.size() << "\r\n";
    for (http::Headers::const_iterator it = headers_.begin(); it != headers_.end(); ++it) {
        response << it->first << ": " << it->second << "\r\n";
    }
    response << "\r\n";
    response << body_;

    return response.str();
}
