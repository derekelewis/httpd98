#ifndef HTTPD98_TEST_H
#define HTTPD98_TEST_H

#include <cstdio>

extern int g_failures;

#define EXPECT_TRUE(cond) \
    do { if(!(cond)) { ++g_failures; \
        std::printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #cond); } } while(0);

#define EXPECT_EQ_STR(expected, actual) \
    do { if(std::string(expected) != std::string(actual)) { ++g_failures; \
        std::printf("FAIL %s:%d expected [%s] got [%s]\n", __FILE__, __LINE__, std::string(expected).c_str(), std::string(actual).c_str()); } } while(0);

#endif
