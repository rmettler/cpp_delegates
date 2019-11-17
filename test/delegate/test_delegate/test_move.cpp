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
                 * doctest::description(
                     "Move delegates with:\n"
                     " - 'rome::delegate<...>::delegate(delegate&&)'\n"
                     " - 'rome::delegate<...>::operator=(delegate&&)'\n"
                     "and test that the new and the remaining instances behave as expected with:\n"
                     " - 'Ret operator()(Args...)'\n"
                     " - 'operator bool()'\n"
                     " - 'operator==' with nullptr\n"
                     " - 'operator!=' with nullptr"));

TEST_CASE("move construction") {
    // TODO: move construct from empty and from any possible target and check the calls!
    //       -> check for empty or non empty
    //       -> check for empty for the one that was moved away
    //       -> check the behavior of the one that was moved away
    CHECK_MESSAGE(false, "not yet written!");
}

TEST_CASE("move assignment") {
    CHECK_MESSAGE(false, "not yet written!");
}

TEST_SUITE_END();
