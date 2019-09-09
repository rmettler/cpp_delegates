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

namespace test_rome_delegate {

TEST_SUITE_BEGIN("header file: rome/delegate.hpp");

bool foo(int i) {
    return i > 0;
}

TEST_CASE("rome::delegate - create") {
    using rome::delegate;
    auto d = delegate<bool(int)>::create<&foo>();
    test_rome_delegate::checkNotEmpty(d);
    CHECK(d(1) == true);
    CHECK(d(-1) == false);
}

TEST_SUITE_END();

}  // namespace test_rome_delegate
