
cmake_minimum_required (VERSION 3.8)

project("def")

include_directories("./inc")

INSTALL (FILES inc/def.h DESTINATION ../install/include/def)