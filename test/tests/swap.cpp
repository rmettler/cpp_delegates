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
using test_vector_assigned = std::tuple<
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<bool(int&), rome::target_is_mandatory>, test::target::function >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::method >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<bool(int&), rome::target_is_mandatory>, test::target::const_method >,
    input_params<     rome::delegate<bool(int&), rome::target_is_expected>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::too_big_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::too_big_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_expected>,  test::target::bad_aligned_functor >,
    input_params<     rome::delegate<bool(int),  rome::target_is_mandatory>, test::target::bad_aligned_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::function >,
    input_params<     rome::delegate<void(int&), rome::target_is_mandatory>, test::target::function >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::function >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::method >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::method >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::method >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::const_method >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::const_method >,
    input_params<     rome::delegate<void(int),  rome::target_is_optional>,  test::target::const_method >,
    input_params<     rome::delegate<void(int),  rome::target_is_expected>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int),  rome::target_is_mandatory>, test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_optional>,  test::target::buffer_optimizable_functor >,
    input_params<     rome::delegate<void(int&), rome::target_is_expected>,  test::target::too_big_functor >,
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

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
TEST_CASE_TEMPLATE_DEFINE("Swap assigned delegates. ", InputParams, swap_assigned) {
    using Delegate   = typename InputParams::delegate;
    using TargetType = typename InputParams::target_type;
    using Sig        = test::delegate_signature_t<Delegate>;


    SUBCASE("Two delegates...") {
        const auto expectDestroyTarget1 =
            test::expectDestroyTargetAtEndOfScope<Sig, TargetType, 1>();
        Delegate dgt0 = test::createDelegateWithMockedTarget<Delegate, TargetType, 0>();
        const auto expectDestroyTarget0 =
            test::expectDestroyTargetAtEndOfScope<Sig, TargetType, 0>();
        Delegate dgt1 = test::createDelegateWithMockedTarget<Delegate, TargetType, 1>();

        SUBCASE("...using member function") {
            STATIC_REQUIRE(noexcept(dgt0.swap(dgt1)));
            dgt0.swap(dgt1);
        }
        SUBCASE("...using global swap") {
            using std::swap;
            STATIC_REQUIRE(noexcept(swap(dgt0, dgt1)));
            swap(dgt0, dgt1);
        }

        CHECK(test::isObservedAsAssigned(dgt0));
        CHECK(test::isObservedAsAssigned(dgt1));
        CHECK(test::callingDelegateCallsMockedTarget<1>(dgt0));
        CHECK(test::callingDelegateCallsMockedTarget<0>(dgt1));
    }
    SUBCASE("Swap with self...") {
        const auto expectDestroyTarget =
            test::expectDestroyTargetAtEndOfScope<Sig, TargetType>();
        Delegate dgt = test::createDelegateWithMockedTarget<Delegate, TargetType>();

        SUBCASE("...using member function") {
            STATIC_REQUIRE(noexcept(dgt.swap(dgt)));
            dgt.swap(dgt);
        }
        SUBCASE("...using global swap") {
            using std::swap;
            STATIC_REQUIRE(noexcept(swap(dgt, dgt)));
            swap(dgt, dgt);
        }

        CHECK(test::isObservedAsAssigned(dgt));
        CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
    }
}
TEST_CASE_TEMPLATE_APPLY(swap_assigned, test_vector_assigned);


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
TEST_CASE_TEMPLATE_DEFINE("Swap assigned with empty delegates. ", InputParams, swap_mixed) {
    using Delegate   = typename InputParams::delegate;
    using TargetType = typename InputParams::target_type;
    using Sig        = test::delegate_signature_t<Delegate>;

    Delegate assigned = test::createDelegateWithMockedTarget<Delegate, TargetType>();
    // target shall be destroyed with delegate 'empty' and before 'assigned'
    const auto expectDestroyTarget = test::expectDestroyTargetAtEndOfScope<Sig, TargetType>();
    Delegate empty{};

    SUBCASE("From assigned to empty...") {
        SUBCASE("...using member function") {
            assigned.swap(empty);
        }
        SUBCASE("...using global swap") {
            using std::swap;
            swap(assigned, empty);
        }
    }
    SUBCASE("From empty to assigned...") {
        SUBCASE("...using member function") {
            empty.swap(assigned);
        }
        SUBCASE("...using global swap") {
            using std::swap;
            swap(empty, assigned);
        }
    }

    CHECK(test::isObservedAsEmpty(assigned));
    CHECK(test::isObservedAsAssigned(empty));
    CHECK(test::callingDelegateLeadsToEmptyBehavior(assigned));
    CHECK(test::callingDelegateCallsMockedTarget<>(empty));
}
TEST_CASE_TEMPLATE_APPLY(swap_mixed, test_vector_mixed);


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
TEST_CASE_TEMPLATE_DEFINE("Swap empty delegates. ", Delegate, swap_empty) {
    SUBCASE("Two empty delegates...") {
        Delegate dgt0{};
        Delegate dgt1{};
        SUBCASE("...using member function") {
            dgt0.swap(dgt1);
        }
        SUBCASE("...using global swap") {
            using std::swap;
            swap(dgt0, dgt1);
        }
        CHECK(test::isObservedAsEmpty(dgt0));
        CHECK(test::isObservedAsEmpty(dgt1));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(dgt0));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(dgt1));
    }
    SUBCASE("Swap with self...") {
        Delegate dgt{};

        SUBCASE("...using member function") {
            dgt.swap(dgt);
        }
        SUBCASE("...using global swap") {
            using std::swap;
            swap(dgt, dgt);
        }

        CHECK(test::isObservedAsEmpty(dgt));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(dgt));
    }
}
TEST_CASE_TEMPLATE_APPLY(swap_empty, test_vector_empty);
