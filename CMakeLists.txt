cmake_minimum_required(VERSION 2.8)

project(denali)
set(CMAKE_BUILD_TYPE Debug)

message(STATUS "The install prefix is ${CMAKE_INSTALL_PREFIX}")
add_definitions(-DINSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_subdirectory(qtgui)
add_subdirectory(ctree)

if(EXISTS "${PROJECT_SOURCE_DIR}/extern/UnitTest++/src/" )
    include_directories(./extern/UnitTest++/src/)
    add_subdirectory(tests)
endif(EXISTS "${PROJECT_SOURCE_DIR}/extern/UnitTest++/src/" )

