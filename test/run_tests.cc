#include "test.h"

#include "request.h"
#include "request_parser.h"

int g_failures = 0;

static void test_no_space_after_colon() {
    RequestParser p;
    Request request = p.ParseRequest("GET / HTTP/1.1\r\nHost:example.com\r\n\r\n");
    EXPECT_TRUE(request.state() == Request::COMPLETE);
    EXPECT_EQ_STR("example.com", request.headers()["host"]);
}

int main(void) {
    test_no_space_after_colon();
    if (g_failures == 0) { std::printf("all tests passed\n"); return 0; };
    std::printf("%d failure(s)\n", g_failures);
    return 1;
}