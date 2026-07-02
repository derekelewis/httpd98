#include "http.h"
#include "request.h"
#include "request_parser.h"

Request RequestParser::ParseRequest(const std::string &request_string) {
    Request request;
    
    std::size_t start, end;
    start = end = 0;

    // parse method & path on 1st line of request string
    end = request_string.find("\r\n", start);
    if (end == std::string::npos) {
        request.state_ = Request::MALFORMED;
        return request;
    }

    // extract & map method
    std::size_t method_end = request_string.find(" ");
    if (method_end > end) {
        request.state_ = Request::MALFORMED;
        return request;
    }
    std::string method_string = request_string.substr(start, method_end - start);
    if (method_string == "OPTIONS") {
        request.method_ = http::OPTIONS;
    } else if (method_string == "GET") {
        request.method_ = http::GET;
    } else if (method_string == "HEAD") {
        request.method_ = http::HEAD;
    } else if (method_string == "POST") {
        request.method_ = http::POST;
    } else if (method_string == "PUT") {
        request.method_ = http::PUT;
    } else if (method_string == "DELETE") {
        request.method_ = http::DELETE;
    } else if (method_string == "TRACE") {
        request.method_ = http::TRACE;
    } else if (method_string == "CONNECT") {
        request.method_ = http::CONNECT;
    } else {
        request.state_ = Request::MALFORMED;
        return request;
    }

    // extract path
    std::size_t path_end = request_string.find(" ", method_end + 1);
    if (path_end > end) {
        request.state_ = Request::MALFORMED;
        return request;
    }
    std::string path_string = request_string.substr(method_end + 1, (path_end - method_end) - 1);
    // TODO: validate path
    request.path_ = path_string;

    // skip first line
    start = end + 2;

    // parse http header names/values
    while ((end = request_string.find("\r\n", start)) != std::string::npos) {
        if (start == end) {
            request.state_ = Request::COMPLETE;
            return request;
        }
        
        std::size_t colon_pos = request_string.find(":", start);
        if (colon_pos >= end) {
            request.state_ = Request::MALFORMED;
            return request;
        }
        // TODO: make key names lowercase
        // TODO: fix colon parsing to allow for empty value headers or no space after colon
        std::string prop_key = request_string.substr(start, (colon_pos - start));
        std::string prop_value = request_string.substr(colon_pos + 2, (end - colon_pos) - 2);
        request.headers_.insert(std::pair<std::string, std::string>(prop_key, prop_value));

        start = end + 2;
    }

    // never reached a blank line so the request is incomplete
    request.state_ = Request::INCOMPLETE;
    
    return request;
}
