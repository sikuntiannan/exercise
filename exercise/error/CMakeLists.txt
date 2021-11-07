
cmake_minimum_required (VERSION 3.8)

project("error")

add_definitions(-D_EXPORTS_)

#include_directories("../install/include")
set(lib_src "error.cpp")

add_library(error STATIC ${lib_src} )

INSTALL (TARGETS error  error DESTINATION lib ARCHIVE DESTINATION lib)

INSTALL (FILES inc/error.h DESTINATION ../install/include/error/)