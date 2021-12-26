
cmake_minimum_required (VERSION 3.8)

project("def")

set(lib_src "def.cpp")

add_definitions(-D_EXPORTS_)
set(CMAKE_DEBUG_POSTFIX "_d")

add_library(def STATIC ${lib_src} )

INSTALL (TARGETS def  def DESTINATION lib ARCHIVE DESTINATION lib)

INSTALL (FILES def.h DESTINATION ../include/def)