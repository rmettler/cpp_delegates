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

#pragma once

namespace test_rome_delegate {
namespace declaration {

    constexpr void test_default_template_params() {
        using rome::delegate;
        static_assert(std::is_same<delegate<bool(int)>, delegate<bool(int), true>>::value, "");
    }

    constexpr void test_declaration_with_correct_signature() {
        using rome::delegate;

        static_assert(!is_signature_error_produced<delegate<bool(int)>>, "");
        static_assert(!is_signature_error_produced<delegate<bool(int), true>>, "");
        static_assert(!is_signature_error_produced<delegate<bool(int), false>>, "");

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

        // Uncomment following line to check the compile error message:
        // delegate<int> d{};

        static_assert(is_signature_error_produced<delegate<int>>, "");
        static_assert(is_signature_error_produced<delegate<int, true>>, "");
        static_assert(is_signature_error_produced<delegate<int, false>>, "");

        static_assert(is_signature_error_produced<delegate<void>>);
        static_assert(is_signature_error_produced<delegate<int(*)[10]>>);
        static_assert(is_signature_error_produced<delegate<int(&)[10]>>);
        static_assert(is_signature_error_produced<delegate<void (*)()>>);
        static_assert(is_signature_error_produced<delegate<void (&)()>>);
        class A {};
        static_assert(is_signature_error_produced<delegate<void (A::*)()>>);
    }

}  // namespace declaration
}  // namespace test_rome_delegate
