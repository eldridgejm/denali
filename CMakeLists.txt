cmake_minimum_required(VERSION 2.8)

project(denali)
set(CMAKE_BUILD_TYPE Debug)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wreturn-type")

include_directories(./extern/UnitTest++/src/)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_subdirectory(tests)
add_subdirectory(qtgui)
add_subdirectory(ctree)
