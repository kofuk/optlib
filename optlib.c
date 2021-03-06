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
#include "config.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#    ifdef HAVE_GETOPT_LONG
#        include <getopt.h>
#    elif defined(HAVE_GETOPT)
#        include <unistd.h>
#    endif
#endif

#include "optlib.h"
#include "optlib_internal.h"

static char *translate_w32_option(char const *long_opt) {
    size_t len = strlen(long_opt);
    size_t n_hyphen = 0;
    for (size_t i = 0; i < len; ++i) {
        if (long_opt[i] == '-') {
            ++n_hyphen;
        }
    }
    char *result = malloc(len - n_hyphen + 1);
    if (!result) return NULL;
    size_t off = 0;
    bool prev_hyphen = true;
    for (size_t i = 0; i <= len; ++i) {
        if (long_opt[i] == '-') {
            prev_hyphen = true;
            continue;
        }
        if (prev_hyphen) {
            result[off++] = (char)toupper(long_opt[i]);
            prev_hyphen = false;
        } else {
            result[off++] = long_opt[i];
        }
    }

    return result;
}

optlib_parser *optlib_parser_new(int argc, char **argv) {
    if (argc <= 0) return NULL;

    optlib_parser *p = malloc(sizeof(optlib_parser));
    if (!p) return NULL;
    memset(p, 0, sizeof(optlib_parser));

    /* duplicate argc and argv */
    p->argc = argc;
#ifdef _WIN32
    p->argc_internal = argc;
#endif
    p->argv = argv;

    p->options = malloc(sizeof(optlib_options));
    if (!p->options) return NULL;
    memset(p->options, 0, sizeof(optlib_options));

#if defined(HAVE_GETOPT_LONG) || defined(HAVE_GETOPT)
    p->opterr = 1;
#endif
    p->optind = 1;

    return p;
}

void optlib_parser_free(optlib_parser *p) {
    for (size_t i = 0; i < p->options->option_count; ++i) {
        free(p->options->options[i].long_opt);
        free(p->options->options[i].description);
#ifdef _WIN32
        free(p->options->options[i].w32_translated);
#endif
        /* I don't free struct optlib_option::argval here
           because it points to somewhere in argment buffer. */
    }
    free(p->options->options);
    free(p->options);
#ifndef _WIN32
#    ifdef HAVE_GETOPT_LONG
    free(p->longopts);
#    endif
#    if defined(HAVE_GETOPT_LONG) || defined(HAVE_GETOPT)
    free(p->shortopts);
#    endif
#endif
}

bool optlib_parser_add_option(optlib_parser *p, char const *long_opt,
                              char const short_opt, bool const has_arg,
                              char const *description) {
    p->initialized = false;

    if (p->options->option_capacity <= p->options->option_count) {
        size_t new_cap;
        if (!p->options->option_capacity) {
            new_cap = 8;
        } else {
            new_cap = p->options->option_capacity << 1;
        }
        optlib_option *new_opts =
            realloc(p->options->options, sizeof(optlib_option) * new_cap);
        if (!new_opts) return false;
        p->options->options = new_opts;
        p->options->option_capacity = new_cap;
    }

    optlib_option *opt = p->options->options + p->options->option_count;
    memset(opt, 0, sizeof(optlib_option));
    if (long_opt) {
        size_t len = strlen(long_opt) + 1;
        opt->long_opt = malloc(len);
        if (!opt->long_opt) return false;
        memcpy(opt->long_opt, long_opt, len);
#ifdef _WIN32
        opt->w32_translated = translate_w32_option(long_opt);
#endif
    }

    opt->short_opt = short_opt;
    opt->has_arg = has_arg;

    if (description) {
        size_t len = strlen(description) + 1;
        opt->description = malloc(len);
        if (!opt->description) return false;
        memcpy(opt->description, description, len);
    }

    p->options->option_count++;
    return true;
}

#ifdef HAVE_GETOPT_LONG
static bool prepare_getopt_long(optlib_parser *p) {
    size_t longcount = 0;
    for (size_t i = 0; i < p->options->option_count; ++i) {
        if (p->options->options[i].long_opt) {
            ++longcount;
        }
    }
    /* for sentinel element */
    ++longcount;

    struct option *new_longopts =
        realloc(p->longopts, sizeof(struct option) * longcount);
    if (!new_longopts) {
        return false;
    }
    p->longopts = new_longopts;
    size_t off = 0;
    for (size_t i = 0; i < p->options->option_count; ++i) {
        if (p->options->options[i].long_opt) {
            p->longopts[off].name = p->options->options[i].long_opt;
            p->longopts[off].has_arg = p->options->options[i].has_arg
                                           ? required_argument
                                           : no_argument;
            p->longopts[off].val = 0;
            p->longopts[off].flag = NULL;
            ++off;
        }
    }
    memset(p->longopts + off, 0, sizeof(struct option));
    return true;
}
#endif

static bool pre_parse_initialize(optlib_parser *p) {
#if !defined(_WIN32) && (defined(HAVE_GETOPT_LONG) || defined(HAVE_GETOPT))
#    ifdef HAVE_GETOPT_LONG
    if (!prepare_getopt_long(p)) {
        return false;
    }
#    endif
    size_t shortlen = 0;
    for (size_t i = 0; i < p->options->option_count; ++i) {
        if (p->options->options[i].short_opt) {
            ++shortlen;
            if (p->options->options[i].has_arg) {
                ++shortlen;
            }
        }
    }

    char *new_shortopts = realloc(p->shortopts, shortlen);
    if (!new_shortopts) {
        return false;
    }
    p->shortopts = new_shortopts;
    size_t off = 0;
    for (size_t i = 0; i < p->options->option_count; ++i) {
        if (p->options->options[i].short_opt) {
            p->shortopts[off++] = p->options->options[i].short_opt;
            if (p->options->options[i].has_arg) {
                p->shortopts[off++] = ':';
            }
        }
    }
    p->shortopts[off] = '\0';
#endif
    return true;
}

optlib_option *optlib_next(optlib_parser *p) {
    if (!p->initialized) {
        if (!pre_parse_initialize(p)) {
            return NULL;
        }
        p->initialized = true;
    }
#if !defined(_WIN32) && (defined(HAVE_GETOPT_LONG) || defined(HAVE_GETOPT))
    optind = p->optind;
    opterr = p->opterr;
#endif
#ifdef _WIN32
retry:
    if (p->optind >= p->argc_internal) {
        p->finished = true;
        return NULL;
    }
    char *this_arg = p->argv[p->optind++];
    if (this_arg[0] == '-') {
        for (size_t i = 0; i < p->options->option_count; ++i) {
            optlib_option *opt = &p->options->options[i];
            if (opt->w32_translated) {
                if (!strcmp(opt->w32_translated, this_arg + 1)) {
                    if (opt->has_arg) {
                        if (p->optind < p->argc) {
                            if (p->argv[p->optind++][0] == '-') {
                                return NULL;
                            } else {
                                opt->argval = p->argv[p->optind - 1];
                                return opt;
                            }
                        } else {
                            p->finished = true;
                            return NULL;
                        }
                    } else {
                        return opt;
                    }
                }
            }
        }
    } else {
        for (int i = p->optind; i < p->argc; ++i) {
            char *tmp = p->argv[i - 1];
            p->argv[i - 1] = p->argv[i];
            p->argv[i] = tmp;
        }
        p->optind--;
#    ifdef _WIN32
        p->argc_internal--;
#    endif
        goto retry;
    }
#else
#    ifdef HAVE_GETOPT_LONG
    int longindex;
    int optc =
        getopt_long(p->argc, p->argv, p->shortopts, p->longopts, &longindex);
    p->opterr = opterr;
    p->optind = optind;
    if (optc == -1) {
        p->finished = true;
        return NULL;
    }
    if (optc == '?' || optc == ':') {
        return NULL;
    } else if (optc != 0) {
        for (size_t i = 0; i < p->options->option_count; ++i) {
            if (p->options->options[i].short_opt == optc) {
                longindex = (int)i;
                break;
            }
        }
    }
    if (p->options->options[longindex].has_arg) {
        if (!optarg) {
            return NULL;
        }
        p->options->options[longindex].argval = optarg;
    }
    return &p->options->options[longindex];
#    elif defined(HAVE_GETOPT)
    int optc = getopt(p->argc, p->argv, p->shortopts);
    p->opterr = opterr;
    p->optind = optind;
    if (optc == -1) {
        p->finished = true;
        return NULL;
    }
    if (optc == '?' || optc == ':') {
        return NULL;
    }
    for (size_t i = 0; i < p->options->option_count; ++i) {
        if (p->options->options[i].short_opt == optc) {
            if (p->options->options[i].has_arg) {
                if (optarg) {
                    p->options->options[i].argval = optarg;
                } else {
                    return NULL;
                }
            }
            return &p->options->options[i];
        }
    }
    return NULL;
#    endif
#endif
    return NULL;
}

void optlib_print_help(optlib_parser *p, FILE *strm) {
#ifdef _WIN32
    size_t padding = 0;
    for (size_t i = 0; i < p->options->option_count; ++i) {
        if (!p->options->options[i].w32_translated) continue;

        size_t length = strlen(p->options->options[i].w32_translated);
        if (p->options->options[i].has_arg) {
            length += 4;
        }
        if (padding < length) {
            padding = length;
        }
    }

    for (size_t i = 0; i < p->options->option_count; ++i) {
        if (!p->options->options[i].w32_translated) continue;

        size_t length = strlen(p->options->options[i].w32_translated);
        fputs("  -", strm);
        fputs(p->options->options[i].w32_translated, strm);
        if (p->options->options[i].has_arg) {
            length += 4;
            fputs(" ARG", strm);
        }
        for (size_t j = length; j < padding; ++j) {
            fputc(' ', strm);
        }
        fprintf(strm, "  %s\n", p->options->options[i].description);
    }
#else
#    ifdef HAVE_GETOPT_LONG
    size_t padding = 0;
    bool have_short = false;
    bool have_short_with_arg = false;
    bool have_long = false;
    for (size_t i = 0; i < p->options->option_count; ++i) {
        size_t length = 0;
        if (p->options->options[i].long_opt) {
            have_long = true;
            length += strlen(p->options->options[i].long_opt) + 2;
            if (p->options->options[i].has_arg) {
                length += 4;
            }
            if (padding < length) {
                padding = length;
            }
        }

        have_short |= p->options->options[i].short_opt;
        have_short_with_arg |=
            p->options->options[i].short_opt && p->options->options[i].has_arg;
    }

    for (size_t i = 0; i < p->options->option_count; ++i) {
        fputs("  ", strm);
        optlib_option opt = p->options->options[i];
        if (opt.short_opt) {
            fprintf(strm, "-%c", opt.short_opt);
            if (opt.has_arg) {
                fputs(" ARG", strm);
            }
            if (opt.long_opt) {
                fputs(", ", strm);
            }
            if (!opt.has_arg && have_short_with_arg) {
                fputs("    ", strm);
            }
        } else {
            if (have_short) {
                if (have_short_with_arg) {
                    fputs("        ", strm);
                } else {
                    fputs("    ", strm);
                }
            }
        }
        size_t current_len = 0;
        if (opt.long_opt) {
            current_len = strlen(opt.long_opt) + 2;
            fprintf(strm, "--%s", opt.long_opt);
            if (opt.has_arg) {
                current_len += 4;
                fputs(" ARG", strm);
            }
        }
        for (size_t j = current_len; j < padding + 2; ++j) {
            fputc(' ', strm);
        }
        if (have_long && opt.short_opt && !opt.long_opt) {
            fputs("  ", strm);
        }
        fprintf(strm, "%s\n", opt.description);
    }

#    elif defined(HAVE_GETOPT)
    bool have_arg = false;
    for (size_t i = 0; i < p->options->option_count; ++i) {
        have_arg |=
            p->options->options[i].short_opt && p->options->options[i].has_arg;
    }

    for (size_t i = 0; i < p->options->option_count; ++i) {
        optlib_option opt = p->options->options[i];
        if (!opt.short_opt) {
            continue;
        }
        char const *fmt;
        if (opt.has_arg) {
            fmt = "  -%c ARG  %s\n";
        } else if (have_arg) {
            fmt = "  -%c      %s\n";
        } else {
            fmt = "  -%c  %s\n";
        }
        fprintf(strm, fmt, opt.short_opt, opt.description);
    }
#    else
#        error "!_WIN32 && !HAVE_GETOPT_LONG && !HAVE_GETOPT"
#    endif
#endif
}

#ifdef TEST
#    include "test_util.h"

int main(void) {
    test_assert(!strcmp(translate_w32_option("foo-bar"), "FooBar"));
    test_assert(!strcmp(translate_w32_option("foo--bar"), "FooBar"));
    test_assert(!strcmp(translate_w32_option("foo-bar-"), "FooBar"));
    test_assert(!strcmp(translate_w32_option("-foo-bar"), "FooBar"));
    test_assert(!strcmp(translate_w32_option("1-2"), "12"));
    test_assert(!strcmp(translate_w32_option("?"), "?"));
    test_assert(!strcmp(translate_w32_option("-"), ""));

#    ifdef _WIN32
    char *argv1[] = {"progname", "-Foo", "bar", 0};
#    elif defined(HAVE_GETOPT_LONG)
    char *argv1[] = {"progname", "--foo", "bar", 0};
#    else
    char *argv1[] = {"progname", "-a", "bar", 0};
#    endif
    optlib_parser *parser = optlib_parser_new(3, argv1);
    optlib_parser_add_option(parser, "foo", 'a', true, "do foo");
    optlib_parser_add_option(parser, "foo-bar", 'b', true, "do foobar");
    optlib_parser_add_option(parser, "foo-bar-baz", 'c', false, "do foobarbaz");
    optlib_parser_add_option(parser, "hoge", 0, false, "do hoge");
    optlib_option *opt = optlib_next(parser);
    test_assert(opt);
    test_assert(!strcmp(opt->long_opt, "foo"));
    test_assert(opt->argval);
    test_assert(!strcmp(opt->argval, "bar"));
    optlib_print_help(parser, stdout);
    optlib_parser_free(parser);

    puts("All tests passed.");
}
#endif
