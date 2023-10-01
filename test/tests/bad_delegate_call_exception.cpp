//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//


#include <rome/delegate.hpp>

#include <doctest/doctest.h>
#include <test/doctest_extensions.hpp>
#include <type_traits>


// NOLINTNEXTLINE(misc-use-anonymous-namespace,readability-function-cognitive-complexity)
TEST_CASE("The bad_delegate_call exception behaves analogous to std::bad_function_call.") {
    STATIC_REQUIRE(std::is_nothrow_default_constructible<rome::bad_delegate_call>{});
    STATIC_REQUIRE(std::is_nothrow_copy_constructible<rome::bad_delegate_call>{});
    STATIC_REQUIRE(std::is_nothrow_copy_assignable<rome::bad_delegate_call>{});
    STATIC_REQUIRE(std::is_nothrow_move_constructible<rome::bad_delegate_call>{});
    STATIC_REQUIRE(std::is_nothrow_move_assignable<rome::bad_delegate_call>{});
    STATIC_REQUIRE(std::is_nothrow_destructible<rome::bad_delegate_call>{});
    STATIC_REQUIRE(std::is_base_of<std::exception, rome::bad_delegate_call>{});
    CHECK_THROWS_WITH_AS(
        throw rome::bad_delegate_call{}, "rome::bad_delegate_call", rome::bad_delegate_call);
}
