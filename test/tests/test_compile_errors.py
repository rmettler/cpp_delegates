from pathlib import Path
import subprocess
from typing import Final
import pytest

test_header: Final = r"""
#include <rome/delegate.hpp>

// Types for testing
enum E : int { e_zero, e_one };
enum class EC : int { zero, one };
union U {
    int i;
};
struct C {
    int i;
    auto memberFunction(int) -> int {
        return ++i;
    }
    auto constMemberFunction(int) const -> int {
        return i;
    }
};
using MemberObject           = int C::*;
using ConstMemberObject      = const int C::*;
using MemberFunctionPtr      = int (C::*)(int);
using ConstMemberFunctionPtr = int (C::*)(int) const;
using Array                  = int[10];
using ArrayRef               = int (&)[10];
using ConstArrayRef          = const int (&)[10];
using ArrayPtr               = int (*)[10];
using ConstArrayPtr          = const int (*)[10];
using FunctionRef            = int (&)(int);
using FunctionPtr            = int (*)(int);

// Test
"""

behavior_types: Final = (
    "rome::target_is_optional",
    "rome::target_is_expected",
    "rome::target_is_mandatory",
)
void_return_and_immutable_args: Final = (
    "void()",
    "void(int)",
    "void(E)",
    "void(EC)",
    "void(U)",
    "void(C)",
    "void(const C&)",
    "void(C&&)",
    "void(const C*)",
    "void(ConstMemberObject)",
    "void(ConstMemberFunctionPtr)",
    "void(ConstArrayRef)",
    "void(ConstArrayPtr)",
    "void(FunctionRef)",
    "void(FunctionPtr)",
)
void_return_and_mutable_args: Final = (
    "void(C&)",
    "void(C*)",
    "void(MemberObject)",
    "void(MemberFunctionPtr)",
    "void(Array)",
    "void(ArrayRef)",
    "void(ArrayPtr)",
)
non_void_return_and_immutable_args: Final = (
    "int()",
    "int(int)",
    "E(E)",
    "EC(EC)",
    "U(U)",
    "C(C)",
    "const C&(const C&)",
    "C&&(C&&)",
    "const C*(const C*)",
    "ConstMemberObject(ConstMemberObject)",
    "ConstMemberFunctionPtr(ConstMemberFunctionPtr)",
    "ConstArrayRef(ConstArrayRef)",
    "ConstArrayPtr(ConstArrayPtr)",
    "FunctionRef(FunctionRef)",
    "FunctionPtr(FunctionPtr)",
)
non_void_return_and_mutable_args: Final = (
    "C&(C&)",
    "C*(C*)",
    "MemberObject(MemberObject)",
    "MemberFunctionPtr(MemberFunctionPtr)",
    "ArrayRef(ArrayRef)",
    "ArrayPtr(ArrayPtr)",
)


def _build_code(test_code: str):
    with Path("tests/compile_errors.cpp").open(mode="w") as file:
        file.write(test_code)
    result = subprocess.run(
        ["ninja", "-C", "../build", "compile_error_tests"],
        capture_output=True,
        text=True,
    )
    return (result.returncode, result.stdout)


@pytest.mark.parametrize("behavior", behavior_types)
def test_signature_is_no_function(behavior: str):
    expected_compile_error: Final = (
        "Invalid parameter 'Signature'. "
        "The template parameter 'Signature' must be a valid function signature."
    )
    test_line = f"rome::delegate<int, {behavior}> dgt;\n"
    print("Test code:")
    print(test_line)
    print("Build output:")
    (returncode, stdout) = _build_code(test_header + test_line)
    print(stdout)
    assert returncode is not 0
    assert expected_compile_error in stdout


@pytest.mark.parametrize(
    "delegate_type,signature",
    [
        ("delegate", sig)
        for sig in void_return_and_immutable_args
        + void_return_and_mutable_args
        + non_void_return_and_immutable_args
        + non_void_return_and_mutable_args
    ]
    + [
        ("fwd_delegate", sig)
        for sig in void_return_and_immutable_args + void_return_and_mutable_args
    ],
)
def test_behavior_is_invalid_type(delegate_type: str, signature: str):
    expected_compile_error: Final = (
        "Invalid parameter 'Behavior'. "
        "The template parameter 'Behavior' must either be empty or contain one of the types "
        "'rome::target_is_optional', 'rome::target_is_expected' or 'rome::target_is_mandatory'."
    )
    test_line = f"rome::{delegate_type}<{signature}, int> dgt;\n"
    print("Test code:")
    print(test_line)
    print("Build output:")
    (returncode, stdout) = _build_code(test_header + test_line)
    print(stdout)
    assert returncode is not 0
    assert expected_compile_error in stdout


@pytest.mark.parametrize(
    "signature", non_void_return_and_immutable_args + non_void_return_and_mutable_args
)
def test_behavior_is_invalid_for_non_void_return(signature: str):
    expected_compile_error: Final = (
        "Return type coflicts with parameter 'Behavior'. "
        "The parameter 'Behavior' is only allowed to be 'rome::target_is_optional' if the return "
        "type is 'void'."
    )
    test_line = f"rome::delegate<{signature}, rome::target_is_optional> dgt;\n"
    print("Test code:")
    print(test_line)
    print("Build output:")
    (returncode, stdout) = _build_code(test_header + test_line)
    print(stdout)
    assert returncode is not 0
    assert expected_compile_error in stdout


@pytest.mark.parametrize("signature", ("int",) + non_void_return_and_immutable_args)
@pytest.mark.parametrize("behavior", behavior_types)
def test_fwd_delegate_return_type_not_void(signature: str, behavior: str):
    expected_compile_error: Final = (
        "Invalid parameter 'Signature'. "
        "The template parameter 'Signature' must be a valid function signature with return type "
        "'void'. Consider using 'rome::delegate' if a non-void return type is needed."
    )
    test_line = f"rome::fwd_delegate<{signature}, {behavior}> dgt;\n"
    print("Test code:")
    print(test_line)
    print("Build output:")
    (returncode, stdout) = _build_code(test_header + test_line)
    print(stdout)
    assert returncode is not 0
    assert expected_compile_error in stdout


@pytest.mark.parametrize(
    "signature", void_return_and_mutable_args + ("void(int, int&)",)
)
@pytest.mark.parametrize("behavior", behavior_types)
def test_fwd_delegate_argument_types_not_immutable(signature: str, behavior: str):
    expected_compile_error: Final = (
        "Invalid mutable function argument in 'void(Args...)'. "
        "All function arguments of a 'rome::fwd_delegate' must be immutable. "
        "The argument types shall prevent that the callee is able to modify passed data still "
        "owned by the caller. E.g. 'int&' is not allowed. "
        "'const int&' is allowed (readonly). 'int' and 'int&&' are also allowed (data owned by callee). "
        "Consider using 'rome::delegate' if mutable arguments are needed."
    )
    test_line = f"rome::fwd_delegate<{signature}, {behavior}> dgt;\n"
    print("Test code:")
    print(test_line)
    print("Build output:")
    (returncode, stdout) = _build_code(test_header + test_line)
    print(stdout)
    assert returncode is not 0
    assert expected_compile_error in stdout
