add_library(doctest INTERFACE IMPORTED)
target_include_directories(doctest SYSTEM INTERFACE
    thirdparty/doctest
)

add_library(trompeloeil INTERFACE IMPORTED)
target_include_directories(trompeloeil SYSTEM INTERFACE
    thirdparty/trompeloeil
)

#-----------------------------------------------------------------------------
# Targets: run_unittests, unittests
add_executable(unittests
    test_main.cpp
    tests/detail/is_immutable_argument.cpp
    tests/type_constraints.cpp
    tests/create_empty.cpp
    tests/create_assigned.cpp
    tests/move_construct.cpp
    tests/move_assign.cpp
    tests/drop_target.cpp
    tests/swap.cpp
    tests/more_function_calls.cpp
    tests/polymorphic_targets.cpp
    tests/command_delegate.cpp
    tests/event_delegate.cpp
    tests/bad_delegate_call_exception.cpp
)
target_include_directories(unittests PRIVATE
    include
)
target_link_libraries(unittests PRIVATE rome_delegates doctest trompeloeil)
if (MSVC)
    target_compile_options(unittests PRIVATE /std:c++14 /W4 /WX)
else()
    target_compile_options(unittests PRIVATE -std=c++14 -fno-rtti -Wall -Wextra -pedantic -Werror)
endif()

add_custom_target(run_unittests
    COMMAND unittests
    USES_TERMINAL
)
add_dependencies(run_unittests unittests)


#-----------------------------------------------------------------------------
# Targets: run_compile_error_tests, compile_error_tests
add_library(compile_error_tests OBJECT
    tests/compile_errors.cpp
)
target_link_libraries(compile_error_tests PRIVATE rome_delegates)
if (MSVC)
    target_compile_options(compile_error_tests PRIVATE /std:c++14)
else()
    target_compile_options(compile_error_tests PRIVATE -std=c++14)
endif()

add_custom_target(run_compile_error_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND pytest tests/test_compile_errors.py
    USES_TERMINAL
)
add_dependencies(run_compile_error_tests rome_delegates)
# the Python script builds the target 'compile_tests'


#-----------------------------------------------------------------------------
# Targets: run_example_tests and example executables

add_custom_target(run_example_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/examples
    DEPENDS example_tests_command
)
add_custom_command(OUTPUT example_tests_command SYMBOLIC
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/examples
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Test examples:"
    USES_TERMINAL
)

find_package(Python3 REQUIRED)

function(add_example_test name)
    set(target_name examples_${name})

    add_executable(${target_name}
        ../examples/${name}.cpp
    )
    target_link_libraries(${target_name} PRIVATE rome_delegates)
    if (MSVC)
        target_compile_options(${target_name} PRIVATE /std:c++14)
    else()
        target_compile_options(${target_name} PRIVATE -std=c++14)
    endif()

    add_custom_command(OUTPUT example_tests_command APPEND
        COMMAND ${Python3_EXECUTABLE} test_examples.py ${name}.cpp $<TARGET_FILE:${target_name}> ${name}_output.txt
        DEPENDS ${target_name}
    )
endfunction()

add_example_test(bad_delegate_call)
add_example_test(basic_examples)
add_example_test(construct)
add_example_test(assign)
add_example_test(empty_after_move)
add_example_test(check_empty)
add_example_test(dtor_destroys_target)
add_example_test(swap)
add_example_test(cruise_control)


#-----------------------------------------------------------------------------
# Target: run_all_tests
add_custom_target(run_all_tests)
add_dependencies(run_all_tests run_unittests run_compile_error_tests run_example_tests)
