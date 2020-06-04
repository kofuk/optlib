#ifndef OPTLIB_H
#define OPTLIB_H

#ifdef HAVE_GETOPT_LONG
#    include <getopt.h>
#endif

#ifdef __cplusplus
#    include <cstddef>
#    include <cstdio>
#else
#    include <stdbool.h>
#    include <stddef.h>
#    include <stdio.h>
#endif

#ifdef __cplusplus
#    define BEGIN_DECL extern "C" {
#    define END_DECL }
#else
#    define BEGIN_DECL
#    define END_DECL
#endif

BEGIN_DECL;

struct optlib_option {
    char *long_opt;
    char short_opt;
    bool has_arg;
    char *description;
    char *argval;
#ifdef _WIN32
    char *w32_translated;
#endif
};

struct optlib_options {
    struct optlib_option *options;
    size_t option_count;
    size_t option_capacity;
};

struct optlib_parser {
    struct optlib_options *options;
    int argc;
    char **argv;
    int optind;
#if defined(HAVE_GETOPT_LONG) || defined(HAVE_GETOPT)
    int opterr;
#endif
    bool initialized;
    bool finished;
#ifndef _WIN32
#    ifdef HAVE_GETOPT_LONG
    struct option *longopts;
#    endif
#    if defined(HAVE_GETOPT_LONG) || defined(HAVE_GETOPT)
    char *shortopts;
#    endif
#endif
};

struct optlib_parser *optlib_parser_new(int argc, char **argv);
void optlib_parser_free(struct optlib_parser *p);
void optlib_parser_add_option(struct optlib_parser *p, char const *long_opt,
                              char const short_opt, bool const has_arg,
                              char const *description);
struct optlib_option *optlib_next(struct optlib_parser *p);
void optlib_print_help(struct optlib_parser *p, FILE *strm);

END_DECL;

#endif
