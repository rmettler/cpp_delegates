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

// helper to check whether the delegate with given signature is of base
// class which would produce a signature error using static_assert during creation
template<typename TDelegate>
constexpr bool is_signature_error_produced =
    std::is_base_of<rome::detail::delegate::signature_error, TDelegate>::value;

template<typename T>
void checkEmpty(const T& dgt) {
    SUBCASE("Check whether given delegate is empty.") {
        CHECK(dgt == nullptr);
        CHECK(nullptr == dgt);
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
}

template<typename T>
void checkNotEmpty(const T& dgt) {
    SUBCASE("Check whether given delegate is not empty.") {
        CHECK(dgt != nullptr);
        CHECK(nullptr != dgt);
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
}

}  // namespace test_rome_delegate
