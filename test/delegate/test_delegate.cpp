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

namespace test_rome_delegate {


// rome::delegate::create
//   - global function
//   - static member function
//   - non-static, non-const/const member function
//   - non-static, non-const/const member object which is callable
//   - function objects
//   - lambda's
// rome::delegate non-static members:
//   - constructors
//   - destructor
//   - operator=
//   - swap
//   - operator bool
//   - operator==, operator!=
//   - operator()
// operator==, operator!=
// rome::swap (also use ADL to test it!)
// rome::make_delegate
//
// With:
//   - different signatures
//   - TargetRequired = [default, true, false]

}  // namespace test_rome_delegate

TEST_SUITE_END();
