//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <doctest.h>

#include <rome/function_ptr.hpp>
#include <type_traits>

TEST_SUITE_BEGIN("header file: rome/function_ptr.hpp");

int function(float f) {
    return static_cast<int>(f);
}

TEST_CASE("rome::function_ptr_t") {
    static_assert(std::is_same<rome::function_ptr_t<int(float)>, decltype(&function)>::value, "");
    rome::function_ptr_t<int(float)> ptr = &function;
    CHECK(3 == (*ptr)(3.9f));
}

struct C {
    bool nonConstMemFn(int i) {
        return i > 0;
    }

    bool constMemFn(int i) const {
        return i < 0;
    }
};

TEST_CASE("rome::member_function_ptr_t") {
    static_assert(
        std::is_same<rome::member_function_ptr_t<C, bool(int)>, decltype(&C::nonConstMemFn)>::value,
        "");
    static_assert(std::is_same<rome::member_function_ptr_t<C, bool(int) const>,
                      decltype(&C::constMemFn)>::value,
        "");

    C c;

    SUBCASE("non-const member function") {
        rome::member_function_ptr_t<C, bool(int)> ptr = &C::nonConstMemFn;
        CHECK(true == (c.*ptr)(3));
    }

    SUBCASE("const member function") {
        rome::member_function_ptr_t<C, bool(int) const> ptr = &C::constMemFn;
        CHECK(false == (c.*ptr)(3));
    }
}

TEST_SUITE_END();  // rome/function_ptr.hpp
