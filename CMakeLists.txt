# optlib --- cross-platform command-line option parser.
# Copyright (C) 2020 Koki Fukuda
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
project(optlib C)
cmake_minimum_required(VERSION 3.15)

enable_testing()

include(CheckSymbolExists)
check_symbol_exists(getopt_long "getopt.h" HAVE_GETOPT_LONG)
check_symbol_exists(getopt "unistd.h" HAVE_GETOPT)

add_library(optlib STATIC optlib.c)

add_executable(optlib_test_builtin optlib.c)
target_compile_definitions(optlib_test_builtin PRIVATE -DTEST)
add_test(NAME optlib_test_builtin COMMAND optlib_test_builtin)

add_executable(optlib_test tests.c)
target_link_libraries(optlib_test PRIVATE optlib)
add_test(NAME optlib_test COMMAND optlib_test)

configure_file(etc/optlib.pc.in optlib.pc @ONLY)
configure_file(config.h.in ${CMAKE_SOURCE_DIR}/config.h)

install(TARGETS optlib DESTINATION lib)
install(FILES optlib.h config.h DESTINATION include/optlib)
install(FILES ${CMAKE_BINARY_DIR}/optlib.pc DESTINATION lib/pkgconfig)
