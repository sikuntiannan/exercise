# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("ALL")

set(CMAKE_DEBUG_POSTFIX "_d")
#set(CMAKE_CXX_STANDARD 20)

# 包含子项目。
add_subdirectory ("def")
add_subdirectory ("error")
add_subdirectory ("auto_ptr")
add_subdirectory ("queue")
add_subdirectory ("exercise")
