//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <doctest.h>

#include <functional>
#include <type_traits>

namespace rome {

template<typename T>
void checkIfDelegateIsEmpty(const T& dgt) {
    SUBCASE("Check if given delegate behaves as empty.") {
        CHECK(dgt == nullptr);
        CHECK(nullptr == dgt);
        CHECK(!dgt);

        const T defaultDgt;
        const T valueInitDgt{};
        const T nullInitDgt{nullptr};
        CHECK(dgt == defaultDgt);
        CHECK(defaultDgt == dgt);
        CHECK(dgt == valueInitDgt);
        CHECK(valueInitDgt == dgt);
        CHECK(dgt == nullInitDgt);
        CHECK(nullInitDgt == dgt);
    }
}

template<typename T>
void checkIfDelegateIsNotEmpty(const T& dgt) {
    SUBCASE("Check if given delegate behaves as not empty.") {
        CHECK(dgt != nullptr);
        CHECK(nullptr != dgt);
        CHECK(!!dgt);

        const T defaultDgt;
        const T valueInitDgt{};
        const T nullInitDgt{nullptr};
        CHECK(dgt != defaultDgt);
        CHECK(defaultDgt != dgt);
        CHECK(dgt != valueInitDgt);
        CHECK(valueInitDgt != dgt);
        CHECK(dgt != nullInitDgt);
        CHECK(nullInitDgt != dgt);
    }
}

template<typename T>
constexpr bool constexprCheckIfDelegateIsEmpty(const T& dgt) {
    // "Check if given delegate behaves as empty during compile time."
    static_assert(dgt == nullptr, "");
    static_assert(nullptr == dgt, "");
    static_assert(!dgt, "");

    const T defaultDgt;
    const T valueInitDgt{};
    const T nullInitDgt{nullptr};
    static_assert(dgt == defaultDgt, "");
    static_assert(defaultDgt == dgt, "");
    static_assert(dgt == valueInitDgt, "");
    static_assert(valueInitDgt == dgt, "");
    static_assert(dgt == nullInitDgt, "");
    static_assert(nullInitDgt == dgt, "");

    return true;
}

template<typename T>
constexpr bool constexprCheckIfDelegateIsNotEmpty(const T& dgt) {
    // "Check if given delegate behaves as not empty during compile time."
    static_assert(dgt != nullptr, "");
    static_assert(nullptr != dgt, "");
    static_assert(!!dgt, "");

    const T defaultDgt;
    const T valueInitDgt{};
    const T nullInitDgt{nullptr};
    static_assert(dgt != defaultDgt, "");
    static_assert(defaultDgt != dgt, "");
    static_assert(dgt != valueInitDgt, "");
    static_assert(valueInitDgt != dgt, "");
    static_assert(dgt != nullInitDgt, "");
    static_assert(nullInitDgt != dgt, "");

    return true;
}

}  // namespace rome
