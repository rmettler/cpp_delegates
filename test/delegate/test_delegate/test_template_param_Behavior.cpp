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

TEST_SUITE_BEGIN("header file: rome/delegate.hpp");

template<typename TDelegate>
constexpr bool produces_expected_behavior_error =
    std::is_base_of<rome::detail::invalid_delegate_expected_behavior, TDelegate>::value;

constexpr bool test_default_type() {
    using rome::delegate;
    static_assert(
        std::is_same<delegate<bool(int)>, delegate<bool(int), rome::target_is_expected>>::value,
        "");
    static_assert(
        std::is_same<delegate<void(int)>, delegate<void(int), rome::target_is_expected>>::value,
        "");
    return true;
}

constexpr bool test_supported_types_do_not_fail() {
    using rome::delegate;
    static_assert(!produces_expected_behavior_error<delegate<bool(int)>>, "");
    static_assert(
        !produces_expected_behavior_error<delegate<bool(int), rome::target_is_expected>>, "");
    static_assert(
        !produces_expected_behavior_error<delegate<bool(int), rome::target_is_mandatory>>, "");
    static_assert(!produces_expected_behavior_error<delegate<void(int)>>, "");
    static_assert(
        !produces_expected_behavior_error<delegate<void(int), rome::target_is_expected>>, "");
    static_assert(
        !produces_expected_behavior_error<delegate<void(int), rome::target_is_mandatory>>, "");
    static_assert(
        !produces_expected_behavior_error<delegate<void(int), rome::target_is_optional>>, "");
    return true;
}

constexpr bool test_unsupported_types_do_fail() {
    using rome::delegate;

    // Uncomment the following lines to check the compile error message:
    // delegate<int> d1{};
    // delegate<bool(int), int> d2{};
    // delegate<bool(int), rome::target_is_optional> d3{};

    static_assert(
        produces_expected_behavior_error<delegate<bool(int), rome::target_is_optional>>, "");
    static_assert(produces_expected_behavior_error<delegate<void(int), int>>, "");
    return true;
}

TEST_CASE("rome::delegate - template parameter 'Behavior'") {
    CHECK(test_default_type());
    CHECK(test_supported_types_do_not_fail());
    CHECK(test_unsupported_types_do_fail());
}

TEST_SUITE_END();  // rome/delegate.hpp
