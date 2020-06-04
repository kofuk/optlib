project(optlib C)
cmake_minimum_required(VERSION 3.15)

include(CheckSymbolExists)
check_symbol_exists(getopt_long "getopt.h" HAVE_GETOPT_LONG)
check_symbol_exists(getopt "unistd.h" HAVE_GETOPT)

add_library(optlib STATIC optlib.c)
if(HAVE_GETOPT_LONG)
  target_compile_definitions(optlib PRIVATE -DHAVE_GETOPT_LONG)
endif()
if(HAVE_GETOPT)
  target_compile_definitions(optlib PRIVATE -DHAVE_GETOPT)
endif()

configure_file(etc/optlib.pc.in optlib.pc @ONLY)

install(TARGETS optlib DESTINATION lib)
install(FILES optlib.h DESTINATION include/optlib)
install(FILES ${CMAKE_BINARY_DIR}/optlib.pc DESTINATION lib/pkgconfig)
