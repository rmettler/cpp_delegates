# Tests whether the compilation success in RESULTS_FILE was as expected by EXPECTATION_FILE and
# if it was not successful, whether the output contained the expected error message.
#
# Variables needed:
#  EXPECTATION_FILE, RESULTS_FILE
cmake_minimum_required(VERSION 3.20)

function(read_file file compile_success_out error_message_out)
    file(STRINGS ${file} file_content)
    list(GET file_content 0 compile_success)
    list(LENGTH file_content len)
    math(EXPR len "${len} - 1")
    if(len GREATER 0)
        list(SUBLIST file_content 1 ${len} error_message_lines)
        list(JOIN error_message_lines "\n" error_message)
    else()
        set(error_message "")
    endif()
    set(${compile_success_out} ${compile_success} PARENT_SCOPE)
    set(${error_message_out} ${error_message} PARENT_SCOPE)
endfunction()

read_file(${EXPECTATION_FILE} expected_compile_success expected_error_message)
read_file(${RESULTS_FILE} compile_success error_message)

if(NOT (expected_compile_success STREQUAL compile_success))
    if(compile_success)
        message(FATAL_ERROR "Expected to compile ${expected_compile_success}, was ${compile_success}.")
    else()
        message(FATAL_ERROR "Expected to compile ${expected_compile_success}, was ${compile_success}.\nOutput: ${error_message}")
    endif()
endif()

if(NOT expected_compile_success)
    string(FIND "${error_message}" "${expected_error_message}" pos)
    if(${pos} EQUAL -1)
        message(FATAL_ERROR "Error message does not match.\nExpected to contain: ${expected_error_message}\nGot: ${error_message}.")
    endif()
endif()