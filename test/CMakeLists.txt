enable_testing()
include(CTest)

#-----------------------------------------------------------------------------
# System includes

add_library(_doctest INTERFACE IMPORTED)
target_include_directories(_doctest SYSTEM INTERFACE
    thirdparty/doctest
)

add_library(_trompeloeil INTERFACE IMPORTED)
target_include_directories(_trompeloeil SYSTEM INTERFACE
    thirdparty/trompeloeil
)


#-----------------------------------------------------------------------------
# Unit test based tests. Test the behavior of the delegates.
# Targets:
#   - run_unittest:
#     Execute the unit tests (target `unittest`).
#     If ROME_DELEGATES_INSTRUMENT is enabled:
#     - Prints errors of address sanitizer and undefined behavior sanitizer to stderr.
#     - Creates coverage data.
#   - coverage:
#     Execute the unit tests, collect coverage results, print the results to console
#     and create coverage reports under {binary_dir}/test/coverage.
#   - clang_tidy:
#     Run clang-tidy code analysis.
#   - unittest:
#     Build the unit tests.
#     Contains coverage instrumentation if `ROME_DELEGATES_INSTRUMENT` is enabled.
#   - _unittest_noinstr:
#     The part of the unit tests that is not instrumented for any analysis.

# Define sources:
set(UNITTEST_SOURCES_TABLE
    # source                                coverage, asan & ubsan instrumentation
    tests/detail/is_immutable_argument.cpp   0
    tests/type_constraints.cpp               0
    tests/create_empty.cpp                   1
    tests/create_assigned.cpp                1
    tests/move_construct.cpp                 1
    tests/move_assign.cpp                    1
    tests/drop_target.cpp                    1
    tests/swap.cpp                           1
    tests/more_function_calls.cpp            1
    tests/polymorphic_targets.cpp            1
    tests/command_delegate.cpp               1
    tests/event_delegate.cpp                 1
    tests/bad_delegate_call_exception.cpp    1
)

function(last_list_index list out_index)
    list(LENGTH ${list} len)
    math(EXPR last "${len} - 1")
    set(${out_index} ${last} PARENT_SCOPE)
endfunction()

last_list_index(UNITTEST_SOURCES_TABLE UNITTEST_SOURCES_TABLE_LAST_INDEX)
foreach(i RANGE 0 ${UNITTEST_SOURCES_TABLE_LAST_INDEX} 2)
    list(GET UNITTEST_SOURCES_TABLE ${i} source)
    list(APPEND UNITTEST_SOURCES ${source})

    math(EXPR i_instr "${i} + 1")
    list(GET UNITTEST_SOURCES_TABLE ${i_instr} instrument)
    if(${instrument})
        list(APPEND UNITTEST_SOURCES_INSTR ${source})
    else()
        list(APPEND UNITTEST_SOURCES_NOINSTR ${source})
    endif()
endforeach()

if(ROME_DELEGATES_INSTRUMENT)
    # With instrumentation enabled, running the unit tests also produces the coverage raw data.
    set(UNITTEST_BYPRODUCTS default.profraw)
endif()


# Add unit tests.
# Targets: run_unittest, unittest, _unittest_noinstr, _doctest_main
add_custom_target(run_unittest
    COMMAND unittest
    BYPRODUCTS ${UNITTEST_BYPRODUCTS}
    USES_TERMINAL
)
add_dependencies(run_unittest unittest)

add_library(_doctest_main OBJECT
    doctest_main.cpp
)
target_link_libraries(_doctest_main PRIVATE _doctest)

add_library(_unittest_noinstr OBJECT ${UNITTEST_SOURCES_NOINSTR})
target_include_directories(_unittest_noinstr PRIVATE include)
target_link_libraries(_unittest_noinstr PRIVATE rome_delegates _doctest)

add_executable(unittest ${UNITTEST_SOURCES_INSTR})
target_include_directories(unittest PRIVATE include)
target_link_libraries(unittest PRIVATE rome_delegates _doctest _trompeloeil _unittest_noinstr _doctest_main)
if(NOT ROME_DELEGATES_INSTRUMENT)
    # If the headers are precompiled the coverage analysis of `rome/delegate.hpp` is missing.
    target_precompile_headers(unittest PRIVATE include/test/common_delegate_checks.hpp)
endif()

if(MSVC)
    target_compile_options(_unittest_noinstr PRIVATE /W4 /WX)
    target_compile_options(unittest PRIVATE /W4 /WX)
else()
    target_compile_options(_unittest_noinstr PRIVATE -fno-rtti -Wall -Wextra -pedantic -Werror)
    target_compile_options(unittest PRIVATE -fno-rtti -Wall -Wextra -pedantic -Werror)
endif()


# Add coverage analysis.
# Targets: coverage
if(ROME_DELEGATES_INSTRUMENT)
    if(NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        message(FATAL_ERROR "Instrumentation for coverage and sanitizers is only available for the LLVM/clang++ compiler.")
    endif()
    target_compile_options(unittest PRIVATE -fprofile-instr-generate -fcoverage-mapping -g -fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-return=always -fsanitize=undefined)
    target_link_options(unittest PRIVATE -fuse-ld=lld -fprofile-instr-generate -fcoverage-mapping -g -fsanitize=address -fsanitize=undefined)
    message(STATUS "LLVM code coverage analysis for target `unittest` enabled.")
    message(STATUS "Address sanitizer for target `unittest` enabled.")
    message(STATUS "Undefined behavior sanitizer for target `unittest` enabled.")

    add_custom_command(
        OUTPUT ut.profdata
        DEPENDS default.profraw
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND llvm-profdata merge -sparse default.profraw -o ut.profdata
    )
    add_custom_command(
        OUTPUT coverage/index.html
        DEPENDS ut.profdata
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND llvm-cov show $<TARGET_FILE:unittest> -instr-profile=ut.profdata --output-dir=coverage -format=html -ignore-filename-regex=test/include/test/.+hpp
                -show-branches=count -show-instantiations=true -show-line-counts=true -show-expansions=true -show-regions=true -Xdemangler c++filt -Xdemangler -n
    )
    add_custom_target(coverage
        DEPENDS coverage/index.html
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND llvm-cov report $<TARGET_FILE:unittest> -instr-profile=ut.profdata -ignore-filename-regex=test/include/test/.+hpp
        USES_TERMINAL
    )
endif()


# Add clang-tidy analysis.
# Targets: clang_tidy, _clang_tidy_compile_commands (dummy target to generate compile_commands.json for clang-tidy)
add_library(_clang_tidy_compile_commands OBJECT ${UNITTEST_SOURCES})
target_include_directories(_clang_tidy_compile_commands PRIVATE include)
target_link_libraries(_clang_tidy_compile_commands PRIVATE rome_delegates _doctest _trompeloeil)
set_target_properties(_clang_tidy_compile_commands PROPERTIES EXPORT_COMPILE_COMMANDS TRUE)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/clang-tidy/compile_commands.json
    DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND cmake -D BINARY_DIR=${CMAKE_BINARY_DIR} -P create-clang-tidy-compile-commands.cmake
)
add_custom_target(clang_tidy
    DEPENDS ${CMAKE_BINARY_DIR}/clang-tidy/compile_commands.json
    DEPENDS ${UNITTEST_SOURCES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND clang-tidy -p ${CMAKE_BINARY_DIR}/clang-tidy ${UNITTEST_SOURCES}
    USES_TERMINAL
)
add_dependencies(clang_tidy rome_delegates _doctest _trompeloeil)


#-----------------------------------------------------------------------------
# Test expected compilation errors.
# Target: run_compile_error_tests
add_subdirectory(compile_errors)


#-----------------------------------------------------------------------------
# Tests that the provided examples are working.
# Target: run_examples_tests
add_subdirectory(examples)
