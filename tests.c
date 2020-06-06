#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "optlib.h"
#include "test_util.h"

static bool test_case_0(void) {
#ifdef _WIN32
    char *argv[] = {"ls",      "foo.c", "-All",           "-Escape", "bar.c",
                    "-Ignore", "*.c",   "-IgnoreBackups", "baz.c",    NULL};
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

    bool has_foo_c = false;
    bool has_bar_c = false;
    bool has_baz_c = false;
    for (int i = parser->optind; i < argc; ++i) {
        if (!strcmp(parser->argv[i], "foo.c")) {
            has_foo_c = true;
        } else if (!strcmp(parser->argv[i], "bar.c")) {
            has_bar_c = true;
        } else if (!strcmp(parser->argv[i], "baz.c")) {
            has_baz_c = true;
        } else {
            test_assert(false);
            return false;
        }
    }
    test_assert(has_foo_c);
    test_assert(has_bar_c);
    test_assert(has_baz_c);
    test_assert(argv[argc] == NULL);

    optlib_parser_free(parser);

    puts("test_case_0 finished normally.");
    return true;
}

int main(void) { return !test_case_0(); }
