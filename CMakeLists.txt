cmake_minimum_required(VERSION 2.8)

project(denali)
set(CMAKE_BUILD_TYPE Debug)

include_directories(./extern/UnitTest++/src/)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_subdirectory(denali)
add_subdirectory(tests)
add_subdirectory(qtgui)
