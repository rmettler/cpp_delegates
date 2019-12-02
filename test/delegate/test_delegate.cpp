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

TEST_SUITE_BEGIN("header file: rome/delegate.hpp");

// rome::delegate non-static members:
//   - constructors
//   - operator=
//   - swap
// rome::swap (also use ADL to test it!)
// rome::make_delegate

TEST_SUITE_END();  // rome/delegate.hpp
