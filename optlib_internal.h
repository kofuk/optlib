#ifndef OPTLIB_INTERNAL_H
#define OPTLIB_INTERNAL_H

#include <stddef.h>

typedef struct optlib_options {
    struct optlib_option *options;
    size_t option_count;
    size_t option_capacity;
} optlib_options;

#endif
