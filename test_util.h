/*
 * optlib --- cross-platform command-line option parser.
 * Copyright (C) 2020 Koki Fukuda
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
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
