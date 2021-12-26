
cmake_minimum_required (VERSION 3.8)

project("error")

add_definitions(-D_EXPORTS_)

include_directories("../def")
link_directories(${CMAKE_INSTALL_PREFIX})

set(lib_src "error.cpp")

add_library(error STATIC ${lib_src} )

target_link_libraries (error LINK_PUBLIC def)

INSTALL (TARGETS error  error DESTINATION lib ARCHIVE DESTINATION lib)

INSTALL (FILES error.h DESTINATION ../include/error/)