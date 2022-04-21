cmake_minimum_required(VERSION 3.12)

include(UseGTest)

# build all project test utility target ----------------------------------------
if(NOT TARGET all_unit_tests)
    add_custom_target(all_unit_tests
        COMMENT "Build all unit tests"
    )
endif()

add_custom_target(${PROJECT_NAME}_all_unit_tests
    COMMENT "Build all ${PROJECT_NAME} unit tests"
)

add_dependencies(all_unit_tests ${PROJECT_NAME}_all_unit_tests)



## Gzat test -------------------------------------------------------------------

add_executable(gzat_test)

target_sources(gzat_test
    PRIVATE
        gzat_test.cpp
)

target_link_libraries(gzat_test
    PRIVATE
        test::gtest::main
        gzat::gzat
)

add_test(
    NAME    ${PROJECT_NAME}_gzat_test
    COMMAND $<TARGET_FILE:gzat_test>
)

add_dependencies(${PROJECT_NAME}_all_unit_tests gzat_test)

# run all project test utility target ------------------------------------------
if(NOT TARGET run_unit_tests)
    add_custom_target(run_unit_tests
        COMMENT "Run all unit tests"
    )
endif()

add_custom_target(run_${PROJECT_NAME}_unit_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -j $(nproc) --tests-regex \"${PROJECT_NAME}_*\" --output-on-failure
    COMMENT "Run all ${PROJECT_NAME} unit tests"
    VERBATIM
)

add_dependencies(run_unit_tests run_${PROJECT_NAME}_unit_tests)