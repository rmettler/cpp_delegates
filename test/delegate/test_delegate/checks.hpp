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

#include <rome/delegate.hpp>
#include <type_traits>


#define ROME_DELEGATE_CHECK_EMPTY(dgt)                                                             \
    static_assert((dgt) == nullptr, "");                                                           \
    static_assert(nullptr == (dgt), "");                                                           \
    static_assert(!((dgt) != nullptr));                                                            \
    static_assert(!(nullptr != (dgt)));                                                            \
    static_assert(!static_cast<bool>(dgt));                                                        \
    static_assert(!(dgt));                                                                         \
    {                                                                                              \
        constexpr decltype(dgt) defaultDgt;                                                        \
        static_assert((dgt) == defaultDgt);                                                        \
        static_assert(defaultDgt == (dgt));                                                        \
        static_assert(!((dgt) != defaultDgt));                                                     \
        static_assert(!(defaultDgt != (dgt)));                                                     \
    }

#define ROME_DELEGATE_CHECK_NOT_EMPTY(dgt)                                                         \
    CHECK(dgt != nullptr);                                                                         \
    static_assert(nullptr != (dgt));                                                               \
    static_assert(!((dgt) == nullptr));                                                            \
    static_assert(!(nullptr == (dgt)));                                                            \
    static_assert(static_cast<bool>(dgt));                                                         \
    static_assert(!!(dgt));                                                                        \
    {                                                                                              \
        constexpr decltype(dgt) defaultDgt;                                                        \
        static_assert(!((dgt) == defaultDgt));                                                     \
        static_assert(!(defaultDgt == (dgt)));                                                     \
        static_assert((dgt) != defaultDgt);                                                        \
        static_assert(defaultDgt != (dgt));                                                        \
    }

namespace test_rome_delegate {

template<typename T>
void checkEmpty(const T& dgt) {
    CHECK(dgt == nullptr);
    CHECK(nullptr == dgt);
    CHECK(!(dgt != nullptr));
    CHECK(!(nullptr != dgt));
    CHECK(!static_cast<bool>(dgt));
    CHECK(!dgt);
    {
        const T defaultDgt;
        CHECK(dgt == defaultDgt);
        CHECK(defaultDgt == dgt);
        CHECK(!(dgt != defaultDgt));
        CHECK(!(defaultDgt != dgt));
    }
}

template<typename T>
void checkNotEmpty(const T& dgt) {
    CHECK(dgt != nullptr);
    CHECK(nullptr != dgt);
    CHECK(!(dgt == nullptr));
    CHECK(!(nullptr == dgt));
    CHECK(static_cast<bool>(dgt));
    CHECK(!!dgt);
    {
        const T defaultDgt;
        CHECK(!(dgt == defaultDgt));
        CHECK(!(defaultDgt == dgt));
        CHECK(dgt != defaultDgt);
        CHECK(defaultDgt != dgt);
    }
}

}  // namespace test_rome_delegate
