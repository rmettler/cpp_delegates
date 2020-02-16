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
#include "mocks.hpp"

TEST_SUITE_BEGIN(
    "header file: rome/delegate.hpp"
    * doctest::description("Create empty delegates with:\n"
                           " - 'rome::delegate<...>::delegate()'\n"
                           " - 'rome::delegate<...>::delegate(std::nullptr_t)'\n"
                           "and test:\n"
                           " - instance is created as expected\n"
                           " - instance is destroyed as expected\n"
                           " - 'Ret operator()(Args...)' works as expected for empty delegates\n"
                           " - 'operator bool()' returns false\n"
                           " - 'operator==' with nullptr returns true\n"
                           " - 'operator!=' with nullptr returns false"));

template<typename TDelegate, typename... Args>
static bool test_default_constructed_no_throw_on_call(Args&&... args) {
    {
        TDelegate dgt;
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_NOTHROW(dgt(std::forward<Args...>(args...)));
    }
    {
        TDelegate dgt{};
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_NOTHROW(dgt(std::forward<Args...>(args...)));
    }
    return true;
}

template<typename TDelegate, typename... Args>
static bool test_default_constructed_throws_on_call(Args&&... args) {
    {
        TDelegate dgt;
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_THROWS_WITH_AS(dgt(std::forward<Args...>(args...)), "rome::bad_delegate_call",
            rome::bad_delegate_call);
    }
    {
        TDelegate dgt{};
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_THROWS_WITH_AS(dgt(std::forward<Args...>(args...)), "rome::bad_delegate_call",
            rome::bad_delegate_call);
    }
    return true;
}

template<typename TDelegate, typename... Args>
static bool test_nullptr_constructed_no_throw_on_call(Args&&... args) {
    {
        TDelegate dgt(nullptr);
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_NOTHROW(dgt(std::forward<Args...>(args...)));
    }
    {
        TDelegate dgt{nullptr};
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_NOTHROW(dgt(std::forward<Args...>(args...)));
    }
    return true;
}

template<typename TDelegate, typename... Args>
static bool test_nullptr_constructed_throws_on_call(Args&&... args) {
    {
        TDelegate dgt(nullptr);
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_THROWS_WITH_AS(dgt(std::forward<Args...>(args...)), "rome::bad_delegate_call",
            rome::bad_delegate_call);
    }
    {
        TDelegate dgt{nullptr};
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_THROWS_WITH_AS(dgt(std::forward<Args...>(args...)), "rome::bad_delegate_call",
            rome::bad_delegate_call);
    }
    return true;
}

static_assert(std::is_same<rome::delegate<void(int)>,
                  rome::delegate<void(int), rome::target_is_expected>>::value,
    "");

static_assert(std::is_same<rome::delegate<bool(int)>,
                  rome::delegate<bool(int), rome::target_is_expected>>::value,
    "");

TEST_CASE("rome::delegate - test default constructed delegates") {
    CHECK(test_default_constructed_no_throw_on_call<
        rome::delegate<void(int), rome::target_is_optional>>(1));
    CHECK(test_default_constructed_throws_on_call<
        rome::delegate<void(int), rome::target_is_expected>>(2));
    CHECK(test_default_constructed_throws_on_call<
        rome::delegate<bool(int), rome::target_is_expected>>(3));
}

TEST_CASE("rome::delegate - test nullptr initialized delegates") {
    CHECK(test_nullptr_constructed_no_throw_on_call<
        rome::delegate<void(int), rome::target_is_optional>>(1));
    CHECK(test_nullptr_constructed_throws_on_call<
        rome::delegate<void(int), rome::target_is_expected>>(2));
    CHECK(test_nullptr_constructed_throws_on_call<
        rome::delegate<bool(int), rome::target_is_expected>>(3));
}

TEST_SUITE_END();
