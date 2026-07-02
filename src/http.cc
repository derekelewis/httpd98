#include "http.h"

std::string http::status_code_phrase(StatusCode status_code)
{
    switch(status_code) {
        case HTTP_200: return "OK";
        case HTTP_400: return "Bad Request";
        case HTTP_403: return "Forbidden";
        case HTTP_404: return "Not Found";
        case HTTP_405: return "Method Not Allowed";
        case HTTP_500: return "Internal Server Error";
        case HTTP_501: return "Not Implemented";
        case HTTP_505: return "HTTP Version Not Supported";
    }
    return "Unknown";
}