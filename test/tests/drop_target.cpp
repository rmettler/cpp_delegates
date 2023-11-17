//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//


#include <rome/delegate.hpp>
#include <test/common_delegate_checks.hpp>


template<typename Delegate, typename TargetType>
struct input_params {
    using delegate    = Delegate;
    using target_type = TargetType;
};

// clang-format off
using test_vector_droppable = std::tuple<
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::none >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::object_optimizable_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::none >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::none >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::function >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::method >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::const_method >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::object_optimizable_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::object_optimizable_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::too_big_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::bad_aligned_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::none >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::none >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::function >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::function >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::const_method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::object_optimizable_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::object_optimizable_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::too_big_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::too_big_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::bad_aligned_functor >
>;
// clang-format on

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters, misc-use-anonymous-namespace)
TEST_CASE_TEMPLATE_DEFINE("Drop the target from a delegate. ", InputParams, drop_target) {
    using Delegate   = typename InputParams::delegate;
    using TargetType = typename InputParams::target_type;
    using Sig        = test::delegate_signature_t<Delegate>;

    Delegate dgt = test::createDelegateWithMockedTarget<Delegate, TargetType>();

    {
        const auto expectDestroyTarget = test::expectDestroyTargetAtEndOfScope<Sig, TargetType>();
        STATIC_REQUIRE(noexcept(dgt = nullptr));
        dgt = nullptr;
    }

    CHECK(test::isObservedAsEmpty(dgt));
    CHECK(test::callingDelegateLeadsToEmptyBehavior(dgt));
}
TEST_CASE_TEMPLATE_APPLY(drop_target, test_vector_droppable);


// clang-format off
using test_vector_not_droppable = std::tuple<
        rome::delegate<bool(int&), rome::target_is_mandatory>,
        rome::delegate<void(int),  rome::target_is_mandatory>,
    rome::fwd_delegate<void(int),  rome::target_is_mandatory>
>;
// clang-format on

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters, misc-use-anonymous-namespace)
TEST_CASE_TEMPLATE_DEFINE(
    "Dropping the target is not possible if target is mandatory. ", Delegate, fail_drop_target) {
    STATIC_REQUIRE(!std::is_assignable<Delegate, decltype(nullptr)>{});
}
TEST_CASE_TEMPLATE_APPLY(fail_drop_target, test_vector_not_droppable);
