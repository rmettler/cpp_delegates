//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <doctest.h>

#include <rome/delegate.hpp>
#include <type_traits>

#include "checks.hpp"
#include "mocks.hpp"

TEST_SUITE_BEGIN("header file: rome/delegate.hpp"
                 * doctest::description("Swap delegates with:\n"
                                        " - 'rome::delegate<...>::swap(delegate&)'\n"
                                        " - 'rome::swap(delegate&, delegate&)'\n"
                                        "and test that both instances behave as expected with:\n"
                                        " - 'Ret operator()(Args...)'\n"
                                        " - 'operator bool()'\n"
                                        " - 'operator==' with nullptr\n"
                                        " - 'operator!=' with nullptr"));

TEST_CASE("TODO1") {
    CHECK_MESSAGE(false, "not yet written!");
}

TEST_CASE("TODO2") {
    CHECK_MESSAGE(false, "not yet written!");
}

TEST_SUITE_END();
