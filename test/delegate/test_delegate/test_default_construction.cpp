//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <doctest.h>

#include <rome/delegate.hpp>
#include <type_traits>

#include "checks.hpp"

TEST_SUITE_BEGIN("header file: rome/delegate.hpp");

// TODO: move construct from empty and from any possible target and check the calls!
//       -> check for empty or non empty
//       -> check for empty for the one that was moved away
//       -> check for behavior for the one that was moved away

template<typename TDelegate>
static constexpr bool test_constexpr_default_construction() {
    TDelegate defaultDgt;
    ROME_DELEGATE_CHECK_EMPTY(defaultDgt);
    TDelegate valueInitDgt{};
    ROME_DELEGATE_CHECK_EMPTY(valueInitDgt);
    return true;
}
template<typename TDelegate>
static bool test_default_construction() {
    constexpr bool result = test_constexpr_default_construction<TDelegate>();
    CHECK_MESSAGE(result, "test_constexpr_default_construction<TDelegate>()");
    TDelegate defaultDgt;
    test_rome_delegate::checkEmpty(defaultDgt);
    TDelegate valueInitDgt{};
    test_rome_delegate::checkEmpty(valueInitDgt);
    return result;
}

template<typename TDelegate>
static constexpr bool test_constexpr_nullptr_construction() {
    TDelegate d1(nullptr);
    ROME_DELEGATE_CHECK_EMPTY(d1);
    TDelegate d2{nullptr};
    ROME_DELEGATE_CHECK_EMPTY(d2);
    return true;
}
template<typename TDelegate>
static bool test_nullptr_construction() {
    constexpr bool result = test_constexpr_nullptr_construction<TDelegate>();
    CHECK_MESSAGE(result, "test_constexpr_nullptr_construction<TDelegate>()");
    TDelegate d1(nullptr);
    test_rome_delegate::checkEmpty(d1);
    TDelegate d2{nullptr};
    test_rome_delegate::checkEmpty(d2);
    return result;
}

TEST_CASE("rome::delegate - default construction") {
    SUBCASE("default construction") {
        CHECK(test_default_construction<rome::delegate<bool(int)>>());
        CHECK(test_default_construction<rome::delegate<bool(int), rome::target_is_expected>>());
        CHECK(test_default_construction<rome::delegate<void(int), rome::target_is_optional>>());
    }
    SUBCASE("nullptr construction") {
        CHECK(test_nullptr_construction<rome::delegate<bool(int)>>());
        CHECK(test_nullptr_construction<rome::delegate<bool(int), rome::target_is_expected>>());
        CHECK(test_nullptr_construction<rome::delegate<void(int), rome::target_is_optional>>());
    }
}

TEST_SUITE_END();
