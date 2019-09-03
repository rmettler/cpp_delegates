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

constexpr void test_default_template_params() {
    using rome::delegate;
    static_assert(
        std::is_same<delegate<bool(int)>, delegate<bool(int), rome::tgt_dyn_req>>::value, "");
    static_assert(
        std::is_same<delegate<void(int)>, delegate<void(int), rome::tgt_dyn_req>>::value, "");
}

constexpr void test_declaration_with_correct_signature() {
    using rome::delegate;
    static_assert(!is_signature_error_produced<delegate<bool(int)>>, "");
    static_assert(!is_target_req_error_produced<delegate<bool(int)>>, "");
    static_assert(!is_signature_error_produced<delegate<bool(int), rome::tgt_dyn_req>>, "");
    static_assert(!is_target_req_error_produced<delegate<bool(int), rome::tgt_dyn_req>>, "");
    static_assert(!is_signature_error_produced<delegate<bool(int), rome::tgt_stat_req>>, "");
    static_assert(!is_target_req_error_produced<delegate<bool(int), rome::tgt_stat_req>>, "");
    static_assert(!is_signature_error_produced<delegate<void(int)>>, "");
    static_assert(!is_target_req_error_produced<delegate<void(int)>>, "");
    static_assert(!is_signature_error_produced<delegate<void(int), rome::tgt_dyn_req>>, "");
    static_assert(!is_target_req_error_produced<delegate<void(int), rome::tgt_dyn_req>>, "");
    static_assert(!is_signature_error_produced<delegate<void(int), rome::tgt_stat_req>>, "");
    static_assert(!is_target_req_error_produced<delegate<void(int), rome::tgt_stat_req>>, "");
    static_assert(!is_signature_error_produced<delegate<void(int), rome::tgt_opt>>, "");
    static_assert(!is_target_req_error_produced<delegate<void(int), rome::tgt_opt>>, "");

    static_assert(!is_signature_error_produced<delegate<void()>>);
    static_assert(!is_signature_error_produced<delegate<void(void)>>);
    static_assert(!is_signature_error_produced<delegate<void(std::nullptr_t)>>);
    static_assert(!is_signature_error_produced<delegate<void(int)>>);
    static_assert(!is_signature_error_produced<delegate<int(void)>>);
    static_assert(!is_signature_error_produced<delegate<int(int)>>);
    static_assert(!is_signature_error_produced<delegate<int(int, int, int, int, int)>>);
}

constexpr void test_declaration_with_wrong_signature() {
    using rome::delegate;

    // Uncomment the following lines to check the compile error message:
    // delegate<int> d1{};
    // delegate<bool(int), int> d2{};
    // delegate<bool(int), rome::tgt_opt> d3{};

    static_assert(is_target_req_error_produced<delegate<bool(int), rome::tgt_opt>>, "");
    static_assert(is_target_req_error_produced<delegate<void(int), int>>, "");

    static_assert(is_signature_error_produced<delegate<int>>, "");
    static_assert(is_signature_error_produced<delegate<int, rome::tgt_dyn_req>>, "");
    static_assert(is_signature_error_produced<delegate<int, rome::tgt_stat_req>>, "");
    static_assert(is_signature_error_produced<delegate<int, rome::tgt_opt>>, "");

    static_assert(is_signature_error_produced<delegate<void>>);
    static_assert(is_signature_error_produced<delegate<int(*)[10]>>);
    static_assert(is_signature_error_produced<delegate<int(&)[10]>>);
    static_assert(is_signature_error_produced<delegate<void (*)()>>);
    static_assert(is_signature_error_produced<delegate<void (&)()>>);
    class A {};
    static_assert(is_signature_error_produced<delegate<void (A::*)()>>);
}

TEST_CASE("rome::delegate - declaration") {
    test_default_template_params();
    test_declaration_with_correct_signature();
    test_declaration_with_wrong_signature();
}

TEST_SUITE_END();

}  // namespace test_rome_delegate
