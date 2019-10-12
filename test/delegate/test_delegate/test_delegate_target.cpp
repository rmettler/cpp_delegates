//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//


#include <doctest.h>

#include <functional>
#include <rome/detail/base_delegate.hpp>
#include <type_traits>

#include "mocks.hpp"

namespace test_rome_delegate {

TEST_SUITE_BEGIN("header file: rome/detail/base_delegate.hpp");

TEST_CASE("base_delegate") {
    // TODO: do this with delegate instead!
    using delegate_type = rome::detail::base_delegate::base_delegate<void(),
        rome::detail::base_delegate::no_call_invoker>;
    SUBCASE("static function") {
        using mock_type     = FunctionMock<void()>;
        mock_type::mockCall = []() {};
        auto& currentCalls  = mock_type::callCounter;
        currentCalls        = 0;
        int expectedCalls   = 0;
        // check proper test setup
        CHECK((currentCalls == expectedCalls));
        {
            auto d1 = delegate_type::create<&mock_type::static_function>();
            CHECK((currentCalls == expectedCalls));
            CHECK(static_cast<bool>(d1));

            d1();
            ++expectedCalls;
            CHECK((currentCalls == expectedCalls));

            auto d2{std::move(d1)};
            CHECK((currentCalls == expectedCalls));
            CHECK(!static_cast<bool>(d1));
            CHECK(static_cast<bool>(d2));

            d1();
            CHECK((currentCalls == expectedCalls));

            d1 = delegate_type{};
            d1();
            CHECK((currentCalls == expectedCalls));

            d2();
            ++expectedCalls;
            CHECK((currentCalls == expectedCalls));
        }
        CHECK((currentCalls == expectedCalls));
    }
    SUBCASE("functor WITH small buffer optimization") {
        using functor_type               = SmallFunctorMock<void()>;
        auto& currentCalls               = functor_type::callCounter;
        currentCalls                     = {};
        FunctorCallCounter expectedCalls = {};
        // check proper test setup
        CHECK(sizeof(functor_type) <= sizeof(void*));
        CHECK(alignof(functor_type) <= sizeof(void*));
        CHECK((currentCalls == expectedCalls));
        {
            functor_type f{[]() {}};
            CHECK((currentCalls == expectedCalls));

            auto d1 = delegate_type::create(f);
            // copy construction and destruction during the temporary object f is passed
            ++expectedCalls.copyConstruction;
            ++expectedCalls.destruction;
            // move construction used to initialize buffer_
            ++expectedCalls.moveConstruction;
            CHECK((currentCalls == expectedCalls));
            CHECK(static_cast<bool>(d1));

            d1();
            ++expectedCalls.callOperator;
            CHECK((currentCalls == expectedCalls));
            {
                auto d2{std::move(d1)};
                CHECK((currentCalls == expectedCalls));
                CHECK(!static_cast<bool>(d1));
                CHECK(static_cast<bool>(d2));

                d1();
                CHECK((currentCalls == expectedCalls));

                d1 = delegate_type{};
                d1();
                CHECK((currentCalls == expectedCalls));

                d2();
                ++expectedCalls.callOperator;
                CHECK((currentCalls == expectedCalls));
            }
            ++expectedCalls.destruction;  // destroys d2
            CHECK((currentCalls == expectedCalls));
        }
        // destruction of d1 doesn't call ~functor_type
        ++expectedCalls.destruction;  // destroys f
        CHECK((currentCalls == expectedCalls));
    }
    SUBCASE("functor too big for small buffer optimization") {
        using functor_type               = BiggerFunctorMock<void()>;
        auto& currentCalls               = functor_type::callCounter;
        currentCalls                     = {};
        FunctorCallCounter expectedCalls = {};
        // check proper test setup
        CHECK(sizeof(functor_type) > sizeof(void*));
        CHECK(alignof(functor_type) <= sizeof(void*));
        CHECK((currentCalls == expectedCalls));
        {
            functor_type f{[]() {}};
            CHECK((currentCalls == expectedCalls));

            auto d1 = delegate_type::create(f);
            // copy construction and destruction during the temporary object f is passed
            ++expectedCalls.copyConstruction;
            ++expectedCalls.destruction;
            // move construction used to initialize buffer_
            ++expectedCalls.newOperator;
            ++expectedCalls.moveConstruction;
            CHECK((currentCalls == expectedCalls));

            d1();
            ++expectedCalls.callOperator;
            CHECK((currentCalls == expectedCalls));
            {
                auto d2{std::move(d1)};
                CHECK((currentCalls == expectedCalls));
                CHECK(!static_cast<bool>(d1));
                CHECK(static_cast<bool>(d2));

                d1();
                CHECK((currentCalls == expectedCalls));

                d1 = delegate_type{};
                d1();
                CHECK((currentCalls == expectedCalls));

                d2();
                ++expectedCalls.callOperator;
                CHECK((currentCalls == expectedCalls));
            }
            ++expectedCalls.destruction;     // destroys d2
            ++expectedCalls.deleteOperator;  // deletes d2
            CHECK((currentCalls == expectedCalls));
        }
        // destruction of d1 doesn't call ~functor_type
        ++expectedCalls.destruction;  // destroys f
        CHECK((currentCalls == expectedCalls));
    }
    SUBCASE("functor too badly aligned for small buffer optimization") {
        using functor_type               = BadAlignedFunctorMock<void()>;
        auto& currentCalls               = functor_type::callCounter;
        currentCalls                     = {};
        FunctorCallCounter expectedCalls = {};
        // check proper test setup
        CHECK(alignof(functor_type) > sizeof(void*));
        CHECK((currentCalls == expectedCalls));
        {
            functor_type f{[]() {}};
            CHECK((currentCalls == expectedCalls));

            auto d1 = delegate_type::create(f);
            // copy construction and destruction during the temporary object f is passed
            ++expectedCalls.copyConstruction;
            ++expectedCalls.destruction;
            // move construction used to initialize buffer_
            ++expectedCalls.newOperator;
            ++expectedCalls.moveConstruction;
            CHECK((currentCalls == expectedCalls));

            d1();
            ++expectedCalls.callOperator;
            CHECK((currentCalls == expectedCalls));
            {
                auto d2{std::move(d1)};
                CHECK((currentCalls == expectedCalls));
                CHECK(!static_cast<bool>(d1));
                CHECK(static_cast<bool>(d2));

                d1();
                CHECK((currentCalls == expectedCalls));

                d1 = delegate_type{};
                d1();
                CHECK((currentCalls == expectedCalls));

                d2();
                ++expectedCalls.callOperator;
                CHECK((currentCalls == expectedCalls));
            }
            ++expectedCalls.destruction;     // destroys d2
            ++expectedCalls.deleteOperator;  // deletes d2
            CHECK((currentCalls == expectedCalls));
        }
        // destruction of d1 doesn't call ~functor_type
        ++expectedCalls.destruction;  // destroys f
        CHECK((currentCalls == expectedCalls));
    }
}

TEST_SUITE_END();

}  // namespace test_rome_delegate
