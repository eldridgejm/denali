include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}
        )

add_executable(denali_tests tests.cpp)
target_link_libraries(denali_tests ${PROJECT_SOURCE_DIR}/extern/UnitTest++/libUnitTest++.a)

add_executable(denali_concepts concepts.cpp)
target_link_libraries(denali_concepts ${PROJECT_SOURCE_DIR}/extern/UnitTest++/libUnitTest++.a)

FOREACH(DATAFILE wenger_vertices wenger_edges wenger_tree)
    configure_file(${DATAFILE} ${CMAKE_CURRENT_BINARY_DIR}/${DATAFILE} COPYONLY)
ENDFOREACH(DATAFILE)
