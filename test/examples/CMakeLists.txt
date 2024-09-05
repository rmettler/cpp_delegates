# Targets:
#  - run_example_tests: build and test all examples
#  - examples: build all examples

file(GLOB example_sources LIST_DIRECTORIES false CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/examples/*.cpp")
foreach(sourcefile ${example_sources})
    cmake_path(GET sourcefile PARENT_PATH path)
    cmake_path(GET sourcefile STEM stem)
    set(target_name example_${stem})
    set(test_name test_${target_name})
    set(expectation_file "${path}/${stem}_output.txt")
    
    add_executable(${target_name}
        ${sourcefile}
    )
    target_link_libraries(${target_name} PRIVATE rome_delegates)

    file(READ "${expectation_file}" exp_output)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${expectation_file})
    string(REGEX REPLACE "([~()])" [[\\\1]] expectation_regex "${exp_output}")  # escape some reserved characters

    add_test(NAME ${test_name} COMMAND ${target_name})
    set_tests_properties(${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "${expectation_regex}")

    list(APPEND example_targets ${target_name})
endforeach()

add_custom_target(examples
    DEPENDS ${example_targets}
)

add_custom_target(run_examples_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS examples
    COMMAND ctest --parallel --output-junit test-results.xml
    USES_TERMINAL
)