#include "request.h"

http::Method Request::method() const
{
    return method_;
}

http::Path Request::path() const
{
    return path_;
}

Request::State Request::state() const
{
    return state_;
}

http::Headers Request::headers() const
{
    return headers_;
}
