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


// NOLINTNEXTLINE(misc-use-anonymous-namespace,readability)
TEST_CASE("The command_delegate is a type alias of a fwd_delegate with target_is_mandatory.") {
    STATIC_REQUIRE(std::is_same<rome::command_delegate<void()>,
        rome::fwd_delegate<void(), rome::target_is_mandatory>>{});
}
