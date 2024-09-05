# Targets:
#  - run_compile_error_tests: test for expected compile errors
#  - compile_error_tests: compiles the sources and saves the results

add_custom_target(run_compile_error_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ctest --progress --output-on-failure --parallel --output-junit test-results.xml
    USES_TERMINAL
)
add_dependencies(run_compile_error_tests compile_error_tests)

define_property(DIRECTORY PROPERTY ALL_RESULTS
    BRIEF_DOCS "List of the files the results of each compilation try."
    FULL_DOCS "List of the files the results of each compilation try."
)

cmake_language(DEFER CALL get_property all_results DIRECTORY PROPERTY ALL_RESULTS)
cmake_language(DEFER CALL add_custom_target
    compile_error_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${all_results}
)


#-----------------------------------------------------------------------------
# Utility functions

function(prepend_zeros in out)
    string(LENGTH ${in} len)
    if(${len} EQUAL 1)
        set(${out} "0${in}" PARENT_SCOPE)
    else()
        set(${out} ${in} PARENT_SCOPE)
    endif()
endfunction()

function(create_test_name test_case subcase_number out)
    prepend_zeros(${subcase_number} suffix)
    set(${out} "${test_case}_${suffix}" PARENT_SCOPE)
endfunction()

function(create_compile_expectation expectation_name expectation_file_out expected_success expected_error)
    set(expectation_file ${expectation_name}.exp)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${expectation_file} "${expected_success}\n${expected_error}")
    set(${expectation_file_out} ${expectation_file} PARENT_SCOPE)
endfunction()

function(add_compile_test test_name expectation_file test_code)
    set(source_file ${test_name}.cpp)
    set(results_file ${test_name}.out)
    set(target test_tgt_${test_name})
    set(test test_${test_name})

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${source_file} "${test_header}${test_code}\n")

    add_library(${target} OBJECT ${source_file})
    target_link_libraries(${target} PRIVATE rome_delegates)

    add_custom_command(
        OUTPUT ${results_file}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${source_file} ${expectation_file}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/compile.cmake
        COMMAND cmake -D BINARY_DIR=${CMAKE_BINARY_DIR} -D TARGET=${target} -D CONFIG=$<CONFIG> -D RESULTS_FILE=${results_file} -P ${CMAKE_CURRENT_SOURCE_DIR}/compile.cmake
    )
    set_property(DIRECTORY APPEND PROPERTY ALL_RESULTS ${results_file})

    add_test(NAME ${test}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND cmake -D EXPECTATION_FILE=${expectation_file} -D RESULTS_FILE=${results_file} -P ${CMAKE_CURRENT_SOURCE_DIR}/test.cmake
    )
endfunction()

function(add_creation_tests test_name expectation_file delegate signature target)
    add_compile_test(${test_name}_factory ${expectation_file}
        "auto dgt = ${delegate}::create(${target});"
    )
    add_compile_test(${test_name}_construct ${expectation_file}
        "${delegate} dgt\{${target}\};"
    )
    string(CONCAT test_code
        "void test() \{\n"
        "    ${delegate} dgt = DummyFunctor<${signature}>();\n"
        "    dgt = ${target};\n"
        "\}"
    )
    add_compile_test(${test_name}_assign ${expectation_file}
        "${test_code}"
    )
endfunction()


#-----------------------------------------------------------------------------
# Test data

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/types.h
    ${CMAKE_CURRENT_BINARY_DIR}/types.h
    COPYONLY
)

set(test_header [[
#include <rome/delegate.hpp>
#include "types.h"

// Test
]])

set(behaviors "rome::target_is_optional" "rome::target_is_expected" "rome::target_is_mandatory")
set(void_return_and_immutable_args 
    "void()"
    "void(int)"
    "void(E)"
    "void(EC)"
    "void(U)"
    "void(C)"
    "void(const C&)"
    "void(C&&)"
    "void(const C*)"
    "void(ConstMemberObject)"
    "void(ConstMemberFunctionPtr)"
    "void(ConstArrayRef)"
    "void(ConstArrayPtr)"
    "void(FunctionRef)"
    "void(FunctionPtr)"
)
set(void_return_and_mutable_args
    "void(C&)"
    "void(C*)"
    "void(MemberObject)"
    "void(MemberFunctionPtr)"
    "void(Array)"
    "void(ArrayRef)"
    "void(ArrayPtr)"
)
set(non_void_return_and_immutable_args
    "int()"
    "int(int)"
    "E(E)"
    "EC(EC)"
    "U(U)"
    "C(C)"
    "const C&(const C&)"
    "C&&(C&&)"
    "const C*(const C*)"
    "ConstMemberObject(ConstMemberObject)"
    "ConstMemberFunctionPtr(ConstMemberFunctionPtr)"
    "ConstArrayRef(ConstArrayRef)"
    "ConstArrayPtr(ConstArrayPtr)"
    "FunctionRef(FunctionRef)"
    "FunctionPtr(FunctionPtr)"
)
set(non_void_return_and_mutable_args
    "C&(C&)"
    "C*(C*)"
    "MemberObject(MemberObject)"
    "MemberFunctionPtr(MemberFunctionPtr)"
    "ArrayRef(ArrayRef)"
    "ArrayPtr(ArrayPtr)"
)


#-----------------------------------------------------------------------------
# Test cases

function(gen_test_signature_is_no_function)
    set(test_case "signature_is_no_function")
    set(expected_success FALSE)
    string(CONCAT expected_error
        "Invalid parameter 'Signature'. "
        "The template parameter 'Signature' must be a valid function signature."
    )
    create_compile_expectation(${test_case} expectation_file ${expected_success} ${expected_error})

    set(subcase_num 0)
    foreach(behavior ${behaviors})
        math(EXPR subcase_num "${subcase_num} + 1")
        create_test_name(${test_case} ${subcase_num} test_name)
        add_compile_test(${test_name} ${expectation_file}
            "rome::delegate<int, ${behavior}> dgt;"
        )
    endforeach()
endfunction()
gen_test_signature_is_no_function()

function(gen_test_behavior_is_invalid_type)
    set(test_case "behavior_is_invalid_type")
    set(expected_success FALSE)
    string(CONCAT expected_error
        "Invalid parameter 'Behavior'. "
        "The template parameter 'Behavior' must either be empty or contain one of the types "
        "'rome::target_is_optional', 'rome::target_is_expected' or 'rome::target_is_mandatory'."
    )
    create_compile_expectation(${test_case} expectation_file ${expected_success} ${expected_error})

    set(subcase_num 0)
    foreach(signature IN LISTS
        void_return_and_immutable_args 
        void_return_and_mutable_args
        non_void_return_and_immutable_args
        non_void_return_and_mutable_args
    )
        math(EXPR subcase_num "${subcase_num} + 1")
        create_test_name(${test_case} ${subcase_num} test_name)
        add_compile_test(${test_name} ${expectation_file}
            "rome::delegate<${signature}, int> dgt;"
        )
    endforeach()
    foreach(signature IN LISTS
        void_return_and_immutable_args 
        void_return_and_mutable_args
    )
        math(EXPR subcase_num "${subcase_num} + 1")
        create_test_name(${test_case} ${subcase_num} test_name)
        add_compile_test(${test_name} ${expectation_file}
            "rome::fwd_delegate<${signature}, int> dgt;"
        )
    endforeach()
endfunction()
gen_test_behavior_is_invalid_type()

function(gen_test_behavior_is_invalid_for_non_void_return)
    set(test_case "behavior_is_invalid_for_non_void_return")
    set(expected_success FALSE)
    string(CONCAT expected_error
        "Return type coflicts with parameter 'Behavior'. "
        "The parameter 'Behavior' is only allowed to be 'rome::target_is_optional' if the return "
        "type is 'void'."
    )
    create_compile_expectation(${test_case} expectation_file ${expected_success} ${expected_error})

    set(subcase_num 0)
    foreach(signature IN LISTS
        non_void_return_and_immutable_args
        non_void_return_and_mutable_args
    )
        math(EXPR subcase_num "${subcase_num} + 1")
        create_test_name(${test_case} ${subcase_num} test_name)
        add_compile_test(${test_name} ${expectation_file}
            "rome::delegate<${signature}, rome::target_is_optional> dgt;"
        )
    endforeach()
endfunction()
gen_test_behavior_is_invalid_for_non_void_return()

function(gen_test_fwd_delegate_return_type_not_void)
    set(test_case "fwd_delegate_return_type_not_void")
    set(expected_success FALSE)
    string(CONCAT expected_error
        "Invalid parameter 'Signature'. "
        "The template parameter 'Signature' must be a valid function signature with return type "
        "'void'. Consider using 'rome::delegate' if a non-void return type is needed."
    )
    create_compile_expectation(${test_case} expectation_file ${expected_success} ${expected_error})

    set(subcase_num 0)
    set(invalid_sig "int")
    foreach(behavior ${behaviors})
        foreach(signature IN LISTS
            invalid_sig
            non_void_return_and_immutable_args
        )
            math(EXPR subcase_num "${subcase_num} + 1")
            create_test_name(${test_case} ${subcase_num} test_name)
            add_compile_test(${test_name} ${expectation_file}
                "rome::fwd_delegate<${signature}, ${behavior}> dgt;"
            )
        endforeach()
    endforeach()
endfunction()
gen_test_fwd_delegate_return_type_not_void()

function(gen_test_fwd_delegate_argument_types_not_immutable)
    set(test_case "fwd_delegate_argument_types_not_immutable")
    set(expected_success FALSE)
    string(CONCAT expected_error
        "Invalid mutable function argument in 'void(Args...)'. "
        "All function arguments of a 'rome::fwd_delegate' must be immutable. "
        "The argument types shall prevent that the callee is able to modify passed data still "
        "owned by the caller. E.g. 'int&' is not allowed. "
        "'const int&' is allowed (readonly). 'int' and 'int&&' are also allowed (data owned by callee). "
        "Consider using 'rome::delegate' if mutable arguments are needed."
    )
    create_compile_expectation(${test_case} expectation_file ${expected_success} ${expected_error})

    set(subcase_num 0)
    set(second_arg_mutable "void(int, int&)")
    foreach(behavior ${behaviors})
        foreach(signature IN LISTS
            void_return_and_mutable_args
            second_arg_mutable
        )
            math(EXPR subcase_num "${subcase_num} + 1")
            create_test_name(${test_case} ${subcase_num} test_name)
            add_compile_test(${test_name} ${expectation_file}
                "rome::fwd_delegate<${signature}, ${behavior}> dgt;"
            )
        endforeach()
    endforeach()
endfunction()
gen_test_fwd_delegate_argument_types_not_immutable()

function(gen_test_create_delegate_with_invalid_functor_type)
    set(test_case "create_delegate_with_invalid_functor_type")
    set(expected_success FALSE)
    string(CONCAT expected_error
        "Invalid object passed. Object needs to be a function object (a class type with a function call "
        "operator, e.g. a lambda)."
    )
    create_compile_expectation(${test_case} expectation_file ${expected_success} ${expected_error})

    set(subcase_num 0)
    foreach(behavior ${behaviors})
        foreach(delegate_type "delegate" "fwd_delegate")
            math(EXPR subcase_num "${subcase_num} + 1")
            create_test_name(${test_case} ${subcase_num} test_name)
            add_creation_tests(${test_name} ${expectation_file}
                "rome::${delegate_type}<void(int), ${behavior}>"  # delegate
                "void(int)"                                       # signature
                "FunctionPtr{nullptr}"                            # target
            )
        endforeach()
    endforeach()
endfunction()
gen_test_create_delegate_with_invalid_functor_type()

function(gen_test_create_delegate_with_functor_of_incompatible_signature)
    set(test_case "create_delegate_with_functor_of_incompatible_signature")
    set(expected_success FALSE)
    string(CONCAT expected_error
        "Passed function object has incompatible function call signature. The function call signature "
        "must be compatible with the signature of the delegate so that the delegate is able to "
        "invoke the function object."
    )
    create_compile_expectation(${test_case} expectation_file ${expected_success} ${expected_error})

    set(subcase_num 0)
    set(delegate_arguments_list "int, int" "int, int" "CFromExplicit, int" "int, CFromExplicit")
    set(target_arguments_list   "C, int"   "int, C"   "C, int"             "int, C")
    foreach(behavior ${behaviors})
        foreach(delegate_type "delegate" "fwd_delegate")
            foreach(delegate_arguments target_arguments 
                IN ZIP_LISTS delegate_arguments_list target_arguments_list
            )
                math(EXPR subcase_num "${subcase_num} + 1")
                create_test_name(${test_case} ${subcase_num} test_name)
                add_creation_tests(${test_name} ${expectation_file}
                    "rome::${delegate_type}<void(${delegate_arguments}), ${behavior}>"  # delegate
                    "void(${delegate_arguments})"                                       # signature
                    "[](${target_arguments}) -> void \{ return; \}"                     # target
                )
            endforeach()
        endforeach()
    endforeach()
    foreach(behavior "rome::target_is_expected" "rome::target_is_mandatory")
        foreach(target_return "int" "CFromExplicit")
            math(EXPR subcase_num "${subcase_num} + 1")
            create_test_name(${test_case} ${subcase_num} test_name)
            add_creation_tests(${test_name} ${expectation_file}
                "rome::delegate<C(), ${behavior}>"             # delegate
                "C()"                                          # signature
                "[]() -> ${target_return} \{ return \{\}; \}"  # target
            )
        endforeach()
    endforeach()
endfunction()
gen_test_create_delegate_with_functor_of_incompatible_signature()

function(gen_test_create_delegate_with_functor_of_compatible_signature)
    set(test_case "create_delegate_with_functor_of_compatible_signature")
    set(expected_success TRUE)
    set(expected_error "IGNORE")
    create_compile_expectation(${test_case} expectation_file ${expected_success} ${expected_error})

    set(subcase_num 0)
    set(delegate_arguments_list "CFrom, int" "int, CFrom")
    set(target_arguments_list   "C, int"     "int, C")
    foreach(behavior ${behaviors})
        foreach(delegate_type "delegate" "fwd_delegate")
            foreach(delegate_arguments target_arguments 
                IN ZIP_LISTS delegate_arguments_list target_arguments_list
            )
                math(EXPR subcase_num "${subcase_num} + 1")
                create_test_name(${test_case} ${subcase_num} test_name)
                add_creation_tests(${test_name} ${expectation_file}
                    "rome::${delegate_type}<void(${delegate_arguments}), ${behavior}>"  # delegate
                    "void(${delegate_arguments})"                                       # signature
                    "[](${target_arguments}) -> void \{ return; \}"                     # target
                )
            endforeach()
        endforeach()
    endforeach()
    set(delegate_signature_list "C(int)" "C(CFrom)")
    set(target_return_list      "CFrom"  "CFrom")
    set(target_arguments_list   "int"    "C")
    foreach(behavior "rome::target_is_expected" "rome::target_is_mandatory")
        foreach(delegate_signature target_return target_arguments
            IN ZIP_LISTS delegate_signature_list target_return_list target_arguments_list
        )
            math(EXPR subcase_num "${subcase_num} + 1")
            create_test_name(${test_case} ${subcase_num} test_name)
            add_creation_tests(${test_name} ${expectation_file}
                "rome::delegate<${delegate_signature}, ${behavior}>"              # delegate
                "${delegate_signature}"                                           # signature
                "[](${target_arguments}) -> ${target_return} \{ return \{\}; \}"  # target
            )
        endforeach()
    endforeach()
endfunction()
gen_test_create_delegate_with_functor_of_compatible_signature()