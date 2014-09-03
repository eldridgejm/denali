cmake_minimum_required(VERSION 2.8)

project(denali)
set(CMAKE_BUILD_TYPE Debug)

message(STATUS "The install prefix is ${CMAKE_INSTALL_PREFIX}")
add_definitions(-DINSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DDENALI_VERSION="v0.1")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -gdwarf-2 -O0")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Qt4 REQUIRED)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_subdirectory(qtgui)
add_subdirectory(ctree)

if(EXISTS "${PROJECT_SOURCE_DIR}/extern/UnitTest++/src/" )
    include_directories(./extern/UnitTest++/src/)
    add_subdirectory(tests)
endif(EXISTS "${PROJECT_SOURCE_DIR}/extern/UnitTest++/src/" )

install(DIRECTORY doc DESTINATION share/denali)
install(DIRECTORY examples DESTINATION share/denali)
install(DIRECTORY pydenali DESTINATION share/denali)
