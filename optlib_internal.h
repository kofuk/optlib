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
#ifndef OPTLIB_INTERNAL_H
#define OPTLIB_INTERNAL_H

#include <stddef.h>

typedef struct optlib_options {
    struct optlib_option *options;
    size_t option_count;
    size_t option_capacity;
} optlib_options;

#endif
