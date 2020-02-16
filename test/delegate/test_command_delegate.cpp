//
// Project: C++ delegates
//
// Copyright Roger Mettler 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <doctest.h>

#include <rome/delegate.hpp>
#include <type_traits>

TEST_SUITE_BEGIN("header file: rome/delegate.hpp");

static_assert(!std::is_default_constructible<rome::command_delegate<void(int)>>::value, "");
static_assert(std::is_base_of<rome::detail::bad_delegate_template_param_with_mutable_arguments,
                  rome::command_delegate<void(int&)>>::value,
    "");


TEST_CASE("rome::command_delegate - construct and call delegate") {
    struct X {
        rome::command_delegate<void(int)> dgt;
    };
    int tst = 0;
    X x = {decltype(X::dgt)::create([&tst](auto i) { tst += i; })};
    CHECK_NOTHROW(x.dgt(13));
    CHECK(tst == 13);
}

TEST_SUITE_END();  // rome/delegate.hpp
