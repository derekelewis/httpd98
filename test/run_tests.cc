#include "test.h"

#include "request.h"
#include "request_parser.h"
#include "configuration_tests.h"

int g_failures = 0;

static void test_no_space_after_colon() {
    RequestParser p;
    Request r = p.ParseRequest("GET / HTTP/1.1\r\nHost:example.com\r\n\r\n");
    EXPECT_TRUE(r.state() == Request::COMPLETE);
    EXPECT_EQ_STR("example.com", r.headers()["host"]);
}

static void test_empty_header_value() {
    RequestParser p;
    Request r = p.ParseRequest("GET / HTTP/1.1\r\nX-Empty:\r\nAccept: text/html\r\n\r\n");
    EXPECT_TRUE(r.state() == Request::COMPLETE);
    EXPECT_EQ_STR("", r.headers()["x-empty"]);
    EXPECT_EQ_STR("text/html", r.headers()["accept"]);
}

int main(void) {
    test_no_space_after_colon();
    test_empty_header_value();
    run_configuration_tests();
    if (g_failures == 0) { std::printf("all tests passed\n"); return 0; }
    std::printf("%d failure(s)\n", g_failures);
    return 1;
}