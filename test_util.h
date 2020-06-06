#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdlib.h>

#ifdef _WIN32
#    define CHECK_MARK_OK "v "
#    define CHECK_MARK_FAILURE "x "
#else
#    define CHECK_MARK_OK "\e[38;5;2m✓\e[0m "
#    define CHECK_MARK_FAILURE "\e[38;5;9m✗\e[0m "
#endif
#define test_assert(expr)                                                   \
    do {                                                                    \
        if (!(expr)) {                                                      \
            printf(CHECK_MARK_FAILURE "Assertion `%s` failed (line: %d)\n", \
                   #expr, __LINE__);                                        \
            exit(1);                                                        \
        } else {                                                            \
            printf(CHECK_MARK_OK "`%s`\n", #expr);                          \
        }                                                                   \
    } while (0);

#endif
