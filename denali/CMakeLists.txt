include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}
        )

add_library(denalicore SHARED
        rectangular_landscape.cpp
        )
