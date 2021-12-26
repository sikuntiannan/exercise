
cmake_minimum_required (VERSION 3.8)

project("auto_ptr")

add_definitions(-D_EXPORTS_)

include_directories("../def")
include_directories("../error")
link_directories(${CMAKE_INSTALL_PREFIX})

set(CMAKE_CXX_STANDARD 20)

set(lib_src "auto_ptr.cpp")

add_library(auto_ptr STATIC ${lib_src} )

target_link_libraries (auto_ptr LINK_PUBLIC def)
target_link_libraries (auto_ptr LINK_PUBLIC error)

INSTALL (TARGETS auto_ptr  auto_ptr DESTINATION lib ARCHIVE DESTINATION lib)

INSTALL (FILES auto_ptr.h DESTINATION ../include/auto_ptr/)