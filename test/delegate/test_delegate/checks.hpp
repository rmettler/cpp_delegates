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
    }
    {
        const T valueInitDgt{};
        CHECK(dgt == valueInitDgt);
        CHECK(valueInitDgt == dgt);
    }
    {
        const T nullInitDgt{nullptr};
        CHECK(dgt == nullInitDgt);
        CHECK(nullInitDgt == dgt);
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
        CHECK(dgt != defaultDgt);
        CHECK(defaultDgt != dgt);
    }
    {
        const T valueInitDgt{};
        CHECK(dgt != valueInitDgt);
        CHECK(valueInitDgt != dgt);
    }
    {
        const T nullInitDgt{nullptr};
        CHECK(dgt != nullInitDgt);
        CHECK(nullInitDgt != dgt);
    }
}

}  // namespace test_rome_delegate
