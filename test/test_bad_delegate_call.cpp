//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <doctest.h>

#include <rome/bad_delegate_call.hpp>
#include <string>
#include <type_traits>

TEST_SUITE_BEGIN("header file: rome/bad_delegate_call.hpp");

TEST_CASE("rome::bad_delegate_call") {
    static_assert(std::is_nothrow_default_constructible<rome::bad_delegate_call>::value, "");
    static_assert(std::is_nothrow_copy_constructible<rome::bad_delegate_call>::value, "");
    static_assert(std::is_nothrow_copy_assignable<rome::bad_delegate_call>::value, "");
    static_assert(std::is_nothrow_move_constructible<rome::bad_delegate_call>::value, "");
    static_assert(std::is_nothrow_move_assignable<rome::bad_delegate_call>::value, "");
    static_assert(std::is_nothrow_destructible<rome::bad_delegate_call>::value, "");

    std::string exceptionString;
    try {
        throw rome::bad_delegate_call{};
    }
    catch (const std::exception& e) {
        exceptionString = e.what();
    }
    CHECK("rome::bad_delegate_call" == exceptionString);
}

TEST_SUITE_END();  // rome/bad_delegate_call.hpp
