
cmake_minimum_required (VERSION 3.8)

project("queue")

add_definitions(-D_EXPORTS_)

set(CMAKE_CXX_STANDARD 20)

#FIND_PACKAGE(Boost)
#IF (Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIR})
#    link_directories(${Boost_LIBRARY_DIRS})
#    ADD_DEFINITIONS( "-DHAS_BOOST" )
#ENDIF()

include_directories("../def")
include_directories("../error")
include_directories("../auto_ptr")
link_directories(${CMAKE_INSTALL_PREFIX})

set(lib_src "queue.cpp")

add_library(queue STATIC ${lib_src} )

target_link_libraries (queue LINK_PUBLIC error)
target_link_libraries (queue LINK_PUBLIC auto_ptr)

INSTALL (TARGETS queue  queue DESTINATION lib ARCHIVE DESTINATION lib)

INSTALL (FILES queue.h DESTINATION ../include/queue/)