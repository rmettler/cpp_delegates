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
using test_vector_from_assigned = std::tuple<
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<bool(int&), rome::target_is_mandatory>, test::target::function >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::const_method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::object_optimizable_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::object_optimizable_functor >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<bool(int&), rome::target_is_mandatory>, test::target::too_big_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::function >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::function >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<void(int&), rome::target_is_mandatory>, test::target::method >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::method >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::const_method >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::const_method >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::object_optimizable_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_mandatory>, test::target::object_optimizable_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::object_optimizable_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::too_big_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::too_big_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_mandatory>, test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::bad_aligned_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::function >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_mandatory>, test::target::function >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::function >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_mandatory>, test::target::method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_mandatory>, test::target::const_method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::const_method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::object_optimizable_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_mandatory>, test::target::object_optimizable_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::object_optimizable_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::too_big_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_mandatory>, test::target::too_big_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::too_big_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_mandatory>, test::target::bad_aligned_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::bad_aligned_functor >
>;
// clang-format on

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters, misc-use-anonymous-namespace)
TEST_CASE_TEMPLATE_DEFINE("Move-construct from a delegate with assigned target. ", InputParams,
    move_construct_from_assigned) {
    using Delegate   = typename InputParams::delegate;
    using TargetType = typename InputParams::target_type;
    using Sig        = test::delegate_signature_t<Delegate>;

    Delegate from            = test::createDelegateWithMockedTarget<Delegate, TargetType>();
    auto expectDestroyTarget = test::expectDestroyTargetAtEndOfScope<Sig, TargetType>();

    STATIC_REQUIRE(noexcept(Delegate(std::move(from))));
    const Delegate to = std::move(from);

    CHECK(test::isObservedAsEmpty(from));  // NOLINT(bugprone-use-after-move)
    CHECK(test::isObservedAsAssigned(to));

    CHECK(test::callingDelegateLeadsToEmptyBehavior(from));
    CHECK(test::callingDelegateCallsMockedTarget<>(to));
}
TEST_CASE_TEMPLATE_APPLY(move_construct_from_assigned, test_vector_from_assigned);


// clang-format off
using test_vector_from_empty = std::tuple<
        rome::delegate<bool(int),  rome::target_is_expected>,
        rome::delegate<void(int&), rome::target_is_expected>,
        rome::delegate<void(int),  rome::target_is_optional>,
    rome::fwd_delegate<void(int),  rome::target_is_expected>,
    rome::fwd_delegate<void(int),  rome::target_is_optional>
>;
// Note: Empty delegates with rome::target_is_mandatory can not be created (see create_empty.cpp).
// clang-format on

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters, misc-use-anonymous-namespace)
TEST_CASE_TEMPLATE_DEFINE(
    "Move-construct from an empty delegate. ", Delegate, move_construct_from_empty) {
    Delegate from{};

    STATIC_REQUIRE(noexcept(Delegate(std::move(from))));
    const Delegate to = std::move(from);

    CHECK(test::isObservedAsEmpty(from));  // NOLINT(bugprone-use-after-move)
    CHECK(test::isObservedAsEmpty(to));

    CHECK(test::callingDelegateLeadsToEmptyBehavior(from));
    CHECK(test::callingDelegateLeadsToEmptyBehavior(to));
}
TEST_CASE_TEMPLATE_APPLY(move_construct_from_empty, test_vector_from_empty);
