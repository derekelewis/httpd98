#include "static_file_handler.h"

#include <ctype.h>
#include <stdlib.h>
#include <string>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <sstream>

#include "http.h"

static std::string content_type_for(const std::string &path)
{
    std::string::size_type dot = path.rfind('.');
    std::string::size_type slash = path.rfind('/');
    if (dot == std::string::npos || (slash != std::string::npos && dot < slash)) {
        return "application/octet-stream";
    }

    std::string extension = path.substr(dot + 1);
    for (std::string::size_type i = 0; i < extension.size(); ++i) {
        extension[i] = static_cast<char>(tolower(extension[i]));
    }

    if (extension == "html" || extension == "htm") return "text/html";
    if (extension == "css") return "text/css";
    if (extension == "js") return "text/javascript";
    if (extension == "txt") return "text/plain";
    if (extension == "json") return "application/json";
    if (extension == "png") return "image/png";
    if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
    if (extension == "gif") return "image/gif";
    if (extension == "svg") return "image/svg+xml";
    if (extension == "ico") return "image/x-icon";
    if (extension == "pdf") return "application/pdf";
    return "application/octet-stream";
}

Response StaticFileHandler::Handle(const Request &request) const
{
    std::string request_path;
    if (http::decode(request.path(), request_path) != http::COMPLETE) {
        return Response(http::HTTP_400, "<html>Bad Request</html>");
    }

    request_path = document_root_ + request_path;

    // realpath requires buffer to be at least PATH_MAX size
    char out[PATH_MAX] = "";
    if (realpath(request_path.c_str(), out) == NULL) {
        return Response(http::HTTP_404, "<html>Not Found</html>");
    }
    std::string request_real_path(out);

    if (http::path_within(document_root_, request_real_path) == false) {
        return Response(http::HTTP_404, "<html>Not Found</html>");
    }
    struct stat status;
    if (stat(request_real_path.c_str(), &status) != 0) {
        return Response(http::HTTP_404, "<html>Not Found</html>");
    }

    // a request for a directory is served from the directory's index.html
    if (S_ISDIR(status.st_mode)) {
        request_real_path += "/index.html";
        // index.html may be a symlink, so re-canonicalize and re-check containment
        if (realpath(request_real_path.c_str(), out) == NULL) {
            return Response(http::HTTP_404, "<html>Not Found</html>");
        }
        request_real_path = out;
        if (http::path_within(document_root_, request_real_path) == false) {
            return Response(http::HTTP_404, "<html>Not Found</html>");
        }
        if (stat(request_real_path.c_str(), &status) != 0) {
            return Response(http::HTTP_404, "<html>Not Found</html>");
        }
    }

    if (S_ISREG(status.st_mode) == false || access(request_real_path.c_str(), R_OK) != 0) {
        return Response(http::HTTP_403, "<html>Forbidden</html>");
    }

    std::ifstream file(request_real_path.c_str(), std::ios::in | std::ios::binary);
    if (file.is_open() == false) {
        return Response(http::HTTP_500, "<html>Internal Server Error</html>");
    }
    std::ostringstream body;
    body << file.rdbuf();

    Response response(http::HTTP_200, body.str());
    response.set_header("Content-Type", content_type_for(request_real_path));
    return response;
}