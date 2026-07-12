#include "test.h"

#include "http.h"

static void test_path_within_1() {
    const std::string base = "/";
    const std::string path = "/etc";
    EXPECT_TRUE(http::path_within(base, path));
}

static void test_path_within_2() {
    const std::string base = "/var/www";
    const std::string path = "/var/wwwevil";
    EXPECT_TRUE(http::path_within(base, path) == false);
}

static void test_path_within_3() {
    const std::string base = "/var/www";
    const std::string path = "/var/www/html";
    EXPECT_TRUE(http::path_within(base, path));  
}

static void test_path_within_4() {
    const std::string base = "/var/www";
    const std::string path = "/var/www/";
    EXPECT_TRUE(http::path_within(base, path));
}

static void test_decode_no_escapes() {
    std::string out;
    EXPECT_TRUE(http::decode("/hello.html", out) == http::COMPLETE);
    EXPECT_EQ_STR("/hello.html", out);
}

static void test_decode_escapes() {
    std::string out;
    EXPECT_TRUE(http::decode("/hello%20world%2Fa%2fb", out) == http::COMPLETE);
    EXPECT_EQ_STR("/hello world/a/b", out);
}

static void test_decode_truncated_escape() {
    std::string out;
    EXPECT_TRUE(http::decode("/hello%2", out) == http::PENDING);
    EXPECT_TRUE(http::decode("/hello%", out) == http::PENDING);
}

static void test_decode_invalid_hex() {
    std::string out;
    EXPECT_TRUE(http::decode("/hello%zz", out) == http::MALFORMED);
}

static void test_decode_embedded_nul() {
    std::string out;
    EXPECT_TRUE(http::decode("/hello%00.html", out) == http::MALFORMED);
}

void run_http_tests() {
    test_path_within_1();
    test_path_within_2();
    test_path_within_3();
    test_path_within_4();
    test_decode_no_escapes();
    test_decode_escapes();
    test_decode_truncated_escape();
    test_decode_invalid_hex();
    test_decode_embedded_nul();
}