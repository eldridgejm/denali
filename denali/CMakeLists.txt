include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}
        )

add_executable(denali cli.cpp)
target_link_libraries(denali ${VTK_LIBRARIES})
