#include "response.h"

#include <sstream>

std::string Response::serialize() const
{
    std::ostringstream response;

    response << "HTTP/1.1 " << status_code_ << " " << http::status_code_phrase(status_code_) << "\r\n";
    // TODO: implement returning a real time
    response << "Date: " << "Thu, 02 Jul 2026 19:13:26 GMT" << "\r\n";
    response << "Content-Type: text/html" << "\r\n";
    response << "Content-Length: " << body_.size() << "\r\n";
    response << "\r\n";
    response << body_;

    return response.str();
}
