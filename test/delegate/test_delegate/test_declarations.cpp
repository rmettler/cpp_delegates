//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <rome/delegate.hpp>
#include <type_traits>

#include "checks.hpp"

namespace test_rome_delegate {

TEST_SUITE_BEGIN("header file: rome/delegate.hpp");

template<typename TDelegate>
constexpr bool produces_expected_behavior_error =
    std::is_base_of<rome::detail::invalid_delegate_expected_behavior, TDelegate>::value;

constexpr void test_default_template_params() {
    using rome::delegate;
    static_assert(
        std::is_same<delegate<bool(int)>, delegate<bool(int), rome::target_is_expected>>::value,
        "");
    static_assert(
        std::is_same<delegate<void(int)>, delegate<void(int), rome::target_is_expected>>::value,
        "");
}

constexpr void test_declaration_with_correct_signature() {
    using rome::delegate;
    static_assert(!produces_expected_behavior_error<delegate<bool(int)>>, "");
    static_assert(!produces_expected_behavior_error<delegate<bool(int), rome::target_is_expected>>, "");
    static_assert(!produces_expected_behavior_error<delegate<bool(int), rome::target_is_mandatory>>, "");
    static_assert(!produces_expected_behavior_error<delegate<void(int)>>, "");
    static_assert(!produces_expected_behavior_error<delegate<void(int), rome::target_is_expected>>, "");
    static_assert(!produces_expected_behavior_error<delegate<void(int), rome::target_is_mandatory>>, "");
    static_assert(!produces_expected_behavior_error<delegate<void(int), rome::target_is_optional>>, "");
}

constexpr void test_declaration_with_wrong_signature() {
    using rome::delegate;

    // Uncomment the following lines to check the compile error message:
    // delegate<int> d1{};
    // delegate<bool(int), int> d2{};
    // delegate<bool(int), rome::target_is_optional> d3{};

    static_assert(produces_expected_behavior_error<delegate<bool(int), rome::target_is_optional>>, "");
    static_assert(produces_expected_behavior_error<delegate<void(int), int>>, "");
}

TEST_CASE("rome::delegate - declaration") {
    test_default_template_params();
    test_declaration_with_correct_signature();
    test_declaration_with_wrong_signature();
}

TEST_SUITE_END();

}  // namespace test_rome_delegate
