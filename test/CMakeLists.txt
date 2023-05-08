# TODO: description

# TODO: add doctest as git subtree
add_library(doctest INTERFACE IMPORTED)
target_sources(doctest INTERFACE
    thirdparty/doctest.h
)
target_include_directories(doctest SYSTEM INTERFACE
    thirdparty
)

set(TEST_NAME test_${PROJECT_NAME})

add_executable(${TEST_NAME}
    test_main.cpp
)
target_link_libraries(${TEST_NAME} PRIVATE rome_delegates doctest)

# General tests
target_sources(${TEST_NAME} PRIVATE
    delegate/test_bad_delegate_call.cpp
    delegate/test_command_delegate.cpp
    delegate/test_event_delegate.cpp
)

# Testing rome::delegate
target_sources(${TEST_NAME} PRIVATE
    delegate/test_delegate/test_assign_nullptr.cpp
    delegate/test_delegate/test_empty_delegates.cpp
    delegate/test_delegate/test_move.cpp
    delegate/test_delegate/test_non_empty_delegates.cpp
    delegate/test_delegate/test_swap.cpp

    delegate/test_delegate/test_template_definition.cpp
    delegate/test_delegate/test_template_param_Behavior.cpp
)

# Testing any forwarding only delegate
target_sources(${TEST_NAME} PRIVATE
    delegate/test_fwd_delegate/test_assign_nullptr.cpp
    delegate/test_fwd_delegate/test_empty_delegates.cpp
    delegate/test_fwd_delegate/test_move.cpp
    delegate/test_fwd_delegate/test_non_empty_delegates.cpp
    delegate/test_fwd_delegate/test_swap.cpp

    delegate/test_fwd_delegate/test_template_definition.cpp
)

if (MSVC)
    target_compile_options(${TEST_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

add_test(NAME unittests COMMAND ${TEST_NAME})

add_custom_target(run_${TEST_NAME}
    COMMAND ${TEST_NAME}
    USES_TERMINAL
)
add_dependencies(run_${TEST_NAME} ${TEST_NAME})
