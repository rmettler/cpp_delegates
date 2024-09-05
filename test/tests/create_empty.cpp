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


// clang-format off
using test_vector_constructible = std::tuple<
        rome::delegate<bool(int),  rome::target_is_expected>,
        rome::delegate<void(int&), rome::target_is_expected>,
        rome::delegate<void(int),  rome::target_is_optional>,
    rome::fwd_delegate<void(int),  rome::target_is_expected>,
    rome::fwd_delegate<void(int),  rome::target_is_optional>
>;
// clang-format on

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
TEST_CASE_TEMPLATE_DEFINE("Construct an empty delegate. ", Delegate, construct_empty) {
    {
        Delegate dgt;
        CHECK(test::isObservedAsEmpty(dgt));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(dgt));
    }
    {
        STATIC_REQUIRE(noexcept(Delegate{}));
        Delegate dgt{};
        CHECK(test::isObservedAsEmpty(dgt));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(dgt));
    }
    {
        STATIC_REQUIRE(noexcept(Delegate(nullptr)));
        Delegate dgt(nullptr);
        CHECK(test::isObservedAsEmpty(dgt));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(dgt));
    }
    {
        STATIC_REQUIRE(noexcept(Delegate{nullptr}));
        Delegate dgt{nullptr};
        CHECK(test::isObservedAsEmpty(dgt));
        CHECK(test::callingDelegateLeadsToEmptyBehavior(dgt));
    }
}
TEST_CASE_TEMPLATE_APPLY(construct_empty, test_vector_constructible);


// clang-format off
using test_vector_not_constructible = std::tuple<
        rome::delegate<bool(int),  rome::target_is_mandatory>,
        rome::delegate<void(int&), rome::target_is_mandatory>,
    rome::fwd_delegate<void(int),  rome::target_is_mandatory>
>;
// clang-format on

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters, misc-use-anonymous-namespace)
TEST_CASE_TEMPLATE_DEFINE("Constructing an empty delegate is not possible if target is mandatory. ",
    Delegate, fail_construct_empty) {
    STATIC_REQUIRE(!std::is_default_constructible<Delegate>{});
    STATIC_REQUIRE(!std::is_constructible<Delegate, decltype(nullptr)>{});
}
TEST_CASE_TEMPLATE_APPLY(fail_construct_empty, test_vector_not_constructible);
