#include "test.h"

#include "configuration.h"

#include <string>

static void test_fromstring() {
    std::string t = "DocumentRoot /var/www/html\nPort 80\n";
    Configuration c = Configuration::FromString(t);
    EXPECT_EQ_STR("/var/www/html", c.document_root());
    unsigned short port = 80;
    EXPECT_TRUE(c.port() == port);
    EXPECT_TRUE(c.state() == Configuration::COMPLETE);
}

static void test_document_root_missing_key() {
    std::string t = "/var/www/html\nPort 80";
    Configuration c = Configuration::FromString(t);
    EXPECT_TRUE(c.state() == Configuration::MALFORMED);
    EXPECT_EQ_STR("Invalid configuration: DocumentRoot not found", c.message());
}

static void test_document_root_missing_value() {
    std::string t = "DocumentRoot\nPort 80\n";
    Configuration c = Configuration::FromString(t);
    EXPECT_TRUE(c.state() == Configuration::MALFORMED);
    EXPECT_EQ_STR("Invalid configuration: DocumentRoot bad format", c.message());
}

static void test_port_missing_key() {
    std::string t = "DocumentRoot /var/www/html\n80";
    Configuration c = Configuration::FromString(t);
    EXPECT_TRUE(c.state() == Configuration::MALFORMED);
    EXPECT_EQ_STR("Invalid configuration: Port not found", c.message());
}

static void test_port_missing_value() {
    std::string t = "DocumentRoot /var/www/html\nPort\n";
    Configuration c = Configuration::FromString(t);
    EXPECT_TRUE(c.state() == Configuration::MALFORMED);
    EXPECT_EQ_STR("Invalid configuration: Port bad format", c.message());
}

static void test_extra_keys() {
    std::string t = "DocumentRoot /var/www/html\nPort 80\nJunk";
    Configuration c = Configuration::FromString(t);
    EXPECT_TRUE(c.state() == Configuration::MALFORMED);
    EXPECT_EQ_STR("Invalid configuration: Unknown keys", c.message());
}

void run_configuration_tests() {
    test_fromstring();
    test_document_root_missing_key();
    test_document_root_missing_value();
    test_port_missing_key();
    test_port_missing_value();
    test_extra_keys();
}