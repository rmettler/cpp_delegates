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

static_assert(std::is_base_of<rome::detail::bad_delegate_template_param_with_mutable_arguments,
                  rome::command_delegate<void(int&)>>::value,
    "");

TEST_CASE("rome::event_delegate - call empty delegate without throwing") {
    auto dgt = rome::event_delegate<void(int)>{};
    CHECK_NOTHROW(dgt(1));
}

TEST_SUITE_END();  // rome/delegate.hpp
