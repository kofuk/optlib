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
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "optlib.h"
#include "test_util.h"

static bool test_case_0(void) {
#ifdef _WIN32
    char *argv[] = {"ls",      "foo.c", "-All",           "-Escape", "bar.c",
                    "-Ignore", "*.c",   "-IgnoreBackups", "baz.c",   NULL};
#elif defined(HAVE_GETOPT_LONG)
    char *argv[] = {"ls",    "foo.c",    "--all", "-b",
                    "bar.c", "--ignore", "*.c",   "--ignore-backups",
                    "baz.c", NULL};
#elif defined(HAVE_GETOPT)
    char *argv[] = {"ls", "foo.c", "-a", "-b",    "bar.c",
                    "-I", "*.c",   "-B", "baz.c", NULL};
#else
    char *argv[] = {NULL};
#endif
    bool all = false;
    bool escape = false;
    char *ignore = NULL;
    bool ignore_backups = false;
    bool directory = false;

    int argc = 9;
    optlib_parser *parser = optlib_parser_new(argc, argv);
    optlib_parser_add_option(parser, "all", 'a', false, "Show hidden files.");
    optlib_parser_add_option(parser, "escape", 'b', false,
                             "Escape nongraphic characters.");
    optlib_parser_add_option(parser, "ignore", 'I', true,
                             "Ignore shell pattern of ARG.");
    optlib_parser_add_option(parser, "ignore-backups", 'B', false,
                             "Ignore text editor's backup files.");
    optlib_parser_add_option(parser, "directory", 'd', false,
                             "List directories.");
    for (;;) {
        optlib_option *opt = optlib_next(parser);
        if (parser->finished) {
            break;
        }
        if (opt == NULL) {
            test_assert(false);

            optlib_print_help(parser, stderr);
            optlib_parser_free(parser);
            return false;
        }
        switch (opt->short_opt) {
        case 'a':
            all = true;
            break;
        case 'b':
            escape = true;
            break;
        case 'I':
            ignore = opt->argval;
            break;
        case 'B':
            ignore_backups = true;
            break;
        case 'd':
            directory = true;
            break;
        }
    }
    test_assert(all);
    test_assert(escape);
    test_assert(!strcmp(ignore, "*.c")) test_assert(ignore);
    test_assert(ignore_backups);
    test_assert(!directory);

    test_assert(parser->optind == 6);
    test_assert(!strcmp(argv[6], "foo.c"));
    test_assert(!strcmp(argv[7], "bar.c"));
    test_assert(!strcmp(argv[8], "baz.c"));

    optlib_parser_free(parser);

    puts("test_case_0 finished normally.");
    return true;
}

bool test_case_1() {
#ifdef _WIN32
    char *argv[] = {"cat",   "-ShowAll", "-UndefinedOption", "foo.c", "bar.c",
                    "baz.c", NULL};
#elif defined(HAVE_GETOPT_LONG)
    char *argv[] = {"cat",   "--show-all", "--undefined-option",
                    "foo.c", "bar.c",      "baz.c",
                    NULL};
#elif defined(HAVE_GETOPT)
    char *argv[] = {"cat", "-A", "-u", "foo.c", "bar.c", "baz.c", NULL};
#else
    char *argv[] = {NULL};
#endif
    int argc = 6;
    optlib_parser *parser = optlib_parser_new(argc, argv);
    optlib_parser_add_option(parser, "show-all", 'A', false,
                             "show nongraphic characters.");

    bool show_all = false;
    int undefined_count = 0;
    for (;;) {
        optlib_option *opt = optlib_next(parser);
        if (parser->finished) {
            break;
        }
        if (!opt) {
#ifdef WIN32
            char *name = "-UndefinedOption";
#elif defined(HAVE_GETOPT_LONG)
            char *name = "--undefined-option";
#elif defined(HAVE_GETOPT)
            char *name = "-u";
#else
            char *name = "";
#endif
            test_assert(!strcmp(parser->argv[parser->optind - 1], name));
            ++undefined_count;
            continue;
        }
        switch (opt->short_opt) {
        case 'A':
            show_all = true;
            break;
        }
    }
    test_assert(undefined_count == 1);
    test_assert(parser->optind == 3);
    test_assert(!strcmp(argv[3], "foo.c"));
    test_assert(!strcmp(argv[4], "bar.c"));
    test_assert(!strcmp(argv[5], "baz.c"));

    optlib_parser_free(parser);
    puts("test_case_1 finished normally.");
    return true;
}

int main(void) {
    bool (*test_cases[])(void) = {&test_case_0, &test_case_1, NULL};
    for (int i = 0;; ++i) {
        if (!test_cases[i]) {
            break;
        }
        test_cases[i]();
    }
}
