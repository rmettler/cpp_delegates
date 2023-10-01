//
// Project: C++ delegates
//
// Copyright Roger Mettler 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//
// Provides extensions to the doctest unittesting framework.

#pragma once

#include <doctest/doctest.h>

// A doctest 'CHECK' macro that additionally returns whether the check has failed or not.
#define CHECK_R(...)                                                                               \
    [&]() -> bool {                                                                                \
        const bool result = (__VA_ARGS__);                                                         \
        if (result) {                                                                              \
            CHECK_UNARY(#__VA_ARGS__);                                                             \
        }                                                                                          \
        else {                                                                                     \
            FAIL_CHECK(#__VA_ARGS__);                                                              \
        }                                                                                          \
        return result;                                                                             \
    }()

// A 'static_assert' check that reports a success to doctest.
#define STATIC_REQUIRE(...)                                                                        \
    static_assert(__VA_ARGS__, #__VA_ARGS__);                                                      \
    CHECK_UNARY(#__VA_ARGS__)
