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


template<typename Delegate, typename TargetType>
struct input_params {
    using delegate    = Delegate;
    using target_type = TargetType;
};


// clang-format off
using test_vector_assigned = std::tuple<
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::function >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<bool(int&), rome::target_is_mandatory>, test::target::method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::const_method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<bool(int&), rome::target_is_mandatory>, test::target::too_big_functor >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::function >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::function >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::method >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::method >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<void(int&), rome::target_is_mandatory>, test::target::const_method >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::const_method >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::too_big_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::too_big_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::bad_aligned_functor >,
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

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
TEST_CASE_TEMPLATE_DEFINE(
    "Move-assign between two delegates with assigned target. ", InputParams, move_assign_assigned) {
    using Delegate   = typename InputParams::delegate;
    using TargetType = typename InputParams::target_type;
    using Sig        = test::delegate_signature_t<Delegate>;

    SUBCASE("Two assigned delegates") {
        const auto expectDestroyTarget1 = test::expectDestroyTargetAtEndOfScope<Sig, TargetType, 1>();
        Delegate from = test::createDelegateWithMockedTarget<Delegate, TargetType, 1>();
        Delegate to   = test::createDelegateWithMockedTarget<Delegate, TargetType, 0>();
        {
            const auto expectDestroyTarget0 =
                test::expectDestroyTargetAtEndOfScope<Sig, TargetType, 0>();
            STATIC_REQUIRE(noexcept(to = std::move(from)));
            to = std::move(from);
        }
        CHECK(test::isObservedAsEmpty(from));  // NOLINT(bugprone-use-after-move)
        CHECK(test::isObservedAsAssigned(to));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(from));
        CHECK(test::callingDelegateCallsMockedTarget<1>(to));
    }
    SUBCASE("Self assign") {
        const auto expectDestroyTarget = test::expectDestroyTargetAtEndOfScope<Sig, TargetType>();
        Delegate dgt = test::createDelegateWithMockedTarget<Delegate, TargetType>();

        DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wself-move")
        // self-move on intention
        dgt = std::move(dgt);
        DOCTEST_GCC_SUPPRESS_WARNING_POP

        CHECK(test::isObservedAsAssigned(dgt));
        CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
    }
}
TEST_CASE_TEMPLATE_APPLY(move_assign_assigned, test_vector_assigned);


// clang-format off
using test_vector_mixed = std::tuple<
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::function >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::method >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::const_method >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::too_big_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::bad_aligned_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::function >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::function >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::const_method >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::buffer_optimizable_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::buffer_optimizable_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::too_big_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::too_big_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params< rome::fwd_delegate<void(int),  rome::target_is_optional>,  test::target::bad_aligned_functor >
>;
// Note: Empty delegates with rome::target_is_mandatory can not be created (see create_empty.cpp).
// clang-format on

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
TEST_CASE_TEMPLATE_DEFINE("Move-assign between an empty and a delegate with assigned target. ",
    InputParams, move_assign_mixed) {
    using Delegate   = typename InputParams::delegate;
    using TargetType = typename InputParams::target_type;
    using Sig        = test::delegate_signature_t<Delegate>;

    SUBCASE("Assigned to empty") {
        const auto expectDestroyTarget = test::expectDestroyTargetAtEndOfScope<Sig, TargetType>();
        Delegate from = test::createDelegateWithMockedTarget<Delegate, TargetType>();
        Delegate to{};

        to = std::move(from);

        CHECK(test::isObservedAsEmpty(from));  // NOLINT(bugprone-use-after-move)
        CHECK(test::isObservedAsAssigned(to));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(from));
        CHECK(test::callingDelegateCallsMockedTarget<>(to));
    }
    SUBCASE("Empty to assigned") {
        Delegate from{};
        Delegate to = test::createDelegateWithMockedTarget<Delegate, TargetType>();
        {
            const auto expectDestroyTarget =
                test::expectDestroyTargetAtEndOfScope<Sig, TargetType>();
            to = std::move(from);
        }
        CHECK(test::isObservedAsEmpty(from));  // NOLINT(bugprone-use-after-move)
        CHECK(test::isObservedAsEmpty(to));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(from));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(to));
    }
}
TEST_CASE_TEMPLATE_APPLY(move_assign_mixed, test_vector_mixed);


// clang-format off
using test_vector_empty = std::tuple<
        rome::delegate<bool(int),  rome::target_is_expected>,
        rome::delegate<void(int),  rome::target_is_expected>,
        rome::delegate<void(int&), rome::target_is_optional>,
    rome::fwd_delegate<void(int),  rome::target_is_expected>,
    rome::fwd_delegate<void(int),  rome::target_is_optional>
>;
// clang-format on

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
TEST_CASE_TEMPLATE_DEFINE(
    "Move-assign an empty to an empty delegate. ", Delegate, move_assign_empty) {
    SUBCASE("Two empty delegates") {
        Delegate from{};
        Delegate to{};

        to = std::move(from);

        CHECK(test::isObservedAsEmpty(from));  // NOLINT(bugprone-use-after-move)
        CHECK(test::isObservedAsEmpty(to));

        CHECK(test::callingDelegateLeadsToEmptyBehavior(from));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(to));
    }
    SUBCASE("Self assign") {
        Delegate dgt{};

        DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wself-move")
        // self-move on intention
        dgt = std::move(dgt);
        DOCTEST_GCC_SUPPRESS_WARNING_POP

        CHECK(test::isObservedAsEmpty(dgt));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(dgt));
    }
}
TEST_CASE_TEMPLATE_APPLY(move_assign_empty, test_vector_empty);
