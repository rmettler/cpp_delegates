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

namespace test_rome_delegate {

TEST_SUITE_BEGIN("header file: rome/delegate.hpp");

TEST_CASE("rome::delegate - create") {
    using rome::delegate;
    using signature_type       = bool(int);
    using mock_type            = FunctionMock<signature_type>;
    mock_type::behavior        = [](int i) { return i > 0; };
    const auto& performedCalls = mock_type::init();
    auto expectedCalls         = performedCalls;
    auto d = delegate<signature_type>::create<&FunctionMock<signature_type>::mockedCall>();
    test_rome_delegate::checkNotEmpty(d);
    CHECK(d(1) == true);
    CHECK(d(-1) == false);
    expectedCalls += 2;
    CHECK(performedCalls == expectedCalls);
}

TEST_SUITE_END();

}  // namespace test_rome_delegate
