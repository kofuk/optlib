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
#ifndef OPTLIB_H
#define OPTLIB_H

#include "config.h"

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

typedef struct optlib_option {
    char *long_opt;
    char short_opt;
    bool has_arg;
    char *description;
    char *argval;
#ifdef _WIN32
    char *w32_translated;
#endif
} optlib_option;

struct optlib_options;

typedef struct optlib_parser {
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
#ifdef _WIN32
    int argc_internal;
#endif
} optlib_parser;

optlib_parser *optlib_parser_new(int argc, char **argv);
void optlib_parser_free(optlib_parser *p);
bool optlib_parser_add_option(optlib_parser *p, char const *long_opt,
                              char const short_opt, bool const has_arg,
                              char const *description);
optlib_option *optlib_next(optlib_parser *p);
void optlib_print_help(optlib_parser *p, FILE *strm);

END_DECL;

#endif
