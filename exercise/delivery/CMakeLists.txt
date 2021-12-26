
cmake_minimum_required (VERSION 3.8)

project("delivery")

add_definitions(-D_EXPORTS_)

set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_CXX_STANDARD 20)

link_directories(${CMAKE_INSTALL_PREFIX})

set(lib_src "delivery.cpp")

add_library(delivery STATIC ${lib_src} )

INSTALL (TARGETS delivery  delivery DESTINATION lib ARCHIVE DESTINATION lib)

INSTALL (FILES delivery.h DESTINATION ../include/delivery/)