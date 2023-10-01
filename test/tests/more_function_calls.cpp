//
// Project: C++ delegates
//
// Copyright Roger Mettler 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <rome/delegate.hpp>
#include <test/common_delegate_checks.hpp>


// NOTE: Basic function call behavior is already tested with create_assigned.cpp and
// create_empty.cpp.

template<typename Delegate, typename TargetType>
struct input_params {
    using delegate    = Delegate;
    using target_type = TargetType;
};


// clang-format off
using test_vector_modifiable_args = std::tuple<
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<bool(int&), rome::target_is_mandatory>, test::target::function >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<bool(int&), rome::target_is_mandatory>, test::target::method >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<bool(int&), rome::target_is_mandatory>, test::target::const_method >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<bool(int&), rome::target_is_mandatory>, test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<bool(int&), rome::target_is_mandatory>, test::target::too_big_functor >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params<     rome::delegate<bool(int&), rome::target_is_mandatory>, test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<void(int&), rome::target_is_mandatory>, test::target::function >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::function >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<void(int&), rome::target_is_mandatory>, test::target::method >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::method >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<void(int&), rome::target_is_mandatory>, test::target::const_method >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::const_method >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_mandatory>, test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_mandatory>, test::target::too_big_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::too_big_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_mandatory>, test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::bad_aligned_functor >
>;
// clang-format on

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters, misc-use-anonymous-namespace)
TEST_CASE_TEMPLATE_DEFINE(
    "When called, the delegate calls the mocked target. ", InputParams, return_by_arg) {
    using Delegate   = typename InputParams::delegate;
    using TargetType = typename InputParams::target_type;
    using Sig        = test::delegate_signature_t<Delegate>;

    auto expectDestroyTarget = test::expectDestroyTargetAtEndOfScope<Sig, TargetType>();
    const Delegate dgt       = test::createDelegateWithMockedTarget<Delegate, TargetType>();

    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
}
TEST_CASE_TEMPLATE_APPLY(return_by_arg, test_vector_modifiable_args);


// clang-format off
using test_vector_all = std::tuple<
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::function >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::const_method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::too_big_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::function >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::function >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::method >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::method >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::const_method >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::const_method >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::too_big_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::too_big_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::bad_aligned_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::function >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_mandatory>, test::target::function >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::function >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_mandatory>, test::target::method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_mandatory>, test::target::const_method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::const_method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::buffer_optimizable_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_mandatory>, test::target::buffer_optimizable_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::buffer_optimizable_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::too_big_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_mandatory>, test::target::too_big_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::too_big_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_mandatory>, test::target::bad_aligned_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::bad_aligned_functor >
>;
// clang-format on

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters, misc-use-anonymous-namespace)
TEST_CASE_TEMPLATE_DEFINE(
    "When called, the delegate forwards any exception thrown by the target to the caller. ",
    InputParams, throw_exception) {
    using Delegate   = typename InputParams::delegate;
    using TargetType = typename InputParams::target_type;
    using Sig        = test::delegate_signature_t<Delegate>;
    using trompeloeil::_;

    auto expectDestroyTarget = test::expectDestroyTargetAtEndOfScope<Sig, TargetType>();
    const Delegate dgt       = test::createDelegateWithMockedTarget<Delegate, TargetType>();

    const int arg = 13;
    REQUIRE_CALL(test::targetMock<Sig>, call(_)).THROW(std::runtime_error("test"));
    CHECK_THROWS_WITH_AS(dgt(arg), "test", std::runtime_error);
}
TEST_CASE_TEMPLATE_APPLY(throw_exception, test_vector_all);
