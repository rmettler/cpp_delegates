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
#include <rome/detail/delegate_base.hpp>
#include <type_traits>

#include "mocks.hpp"

namespace test_rome_delegate {

TEST_SUITE_BEGIN("header file: rome/detail/delegate_base.hpp");

TEST_CASE("delegate_base") {
    using delegate_type = rome::detail::delegate_base::delegate_base<void(),
        rome::detail::delegate_base::no_call_invoker>;
    SUBCASE("static function") {
        using mock_type            = FunctionMock<void()>;
        mock_type::behavior        = []() {};
        const auto& performedCalls = mock_type::init();
        auto expectedCalls         = performedCalls;
        // check proper test setup
        CHECK((performedCalls == expectedCalls));
        {
            auto d1 = delegate_type::create<&mock_type::mockedCall>();
            CHECK((performedCalls == expectedCalls));
            CHECK(static_cast<bool>(d1));

            d1();
            ++expectedCalls.callOperator;
            CHECK((performedCalls == expectedCalls));

            auto d2{std::move(d1)};
            CHECK((performedCalls == expectedCalls));
            CHECK(!static_cast<bool>(d1));
            CHECK(static_cast<bool>(d2));

            d1();
            CHECK((performedCalls == expectedCalls));

            d1 = delegate_type{};
            d1();
            CHECK((performedCalls == expectedCalls));

            d2();
            ++expectedCalls.callOperator;
            CHECK((performedCalls == expectedCalls));
        }
        CHECK((performedCalls == expectedCalls));
    }
    SUBCASE("functor WITH small buffer optimization") {
        using mock_type            = SmallFunctorMock<void()>;
        mock_type::behavior        = []() {};
        const auto& performedCalls = mock_type::init();
        auto expectedCalls         = performedCalls;
        CHECK(sizeof(mock_type) <= sizeof(void*));
        CHECK(alignof(mock_type) <= sizeof(void*));
        CHECK((performedCalls == expectedCalls));
        {
            mock_type f{};
            ++expectedCalls.defaultConstruction;
            CHECK((performedCalls == expectedCalls));

            auto d1 = delegate_type::create(f);
            ++expectedCalls.copyConstruction;
            CHECK((performedCalls == expectedCalls));
            CHECK(static_cast<bool>(d1));

            d1();
            ++expectedCalls.callOperator;
            CHECK((performedCalls == expectedCalls));
            {
                auto d2{std::move(d1)};
                CHECK((performedCalls == expectedCalls));
                CHECK(!static_cast<bool>(d1));
                CHECK(static_cast<bool>(d2));

                d1();
                CHECK((performedCalls == expectedCalls));

                d1 = delegate_type{};
                d1();
                CHECK((performedCalls == expectedCalls));

                d2();
                ++expectedCalls.callOperator;
                CHECK((performedCalls == expectedCalls));
            }
            ++expectedCalls.destruction;  // destroys d2
            CHECK((performedCalls == expectedCalls));
        }
        // destruction of d1 doesn't call ~mock_type
        ++expectedCalls.destruction;  // destroys f
        CHECK((performedCalls == expectedCalls));
    }
    SUBCASE("functor too big for small buffer optimization") {
        using mock_type            = BiggerFunctorMock<void()>;
        mock_type::behavior        = []() {};
        const auto& performedCalls = mock_type::init();
        auto expectedCalls         = performedCalls;
        // check proper test setup
        CHECK(sizeof(mock_type) > sizeof(void*));
        CHECK(alignof(mock_type) <= sizeof(void*));
        CHECK((performedCalls == expectedCalls));
        {
            mock_type f{};
            ++expectedCalls.defaultConstruction;
            CHECK((performedCalls == expectedCalls));

            auto d1 = delegate_type::create(f);
            ++expectedCalls.copyConstruction;
            ++expectedCalls.newOperator;
            CHECK((performedCalls == expectedCalls));

            d1();
            ++expectedCalls.callOperator;
            CHECK((performedCalls == expectedCalls));
            {
                auto d2{std::move(d1)};
                CHECK((performedCalls == expectedCalls));
                CHECK(!static_cast<bool>(d1));
                CHECK(static_cast<bool>(d2));

                d1();
                CHECK((performedCalls == expectedCalls));

                d1 = delegate_type{};
                d1();
                CHECK((performedCalls == expectedCalls));

                d2();
                ++expectedCalls.callOperator;
                CHECK((performedCalls == expectedCalls));
            }
            ++expectedCalls.destruction;     // destroys d2
            ++expectedCalls.deleteOperator;  // deletes d2
            CHECK((performedCalls == expectedCalls));
        }
        // destruction of d1 doesn't call ~mock_type
        ++expectedCalls.destruction;  // destroys f
        CHECK((performedCalls == expectedCalls));
    }
    SUBCASE("functor too badly aligned for small buffer optimization") {
        using mock_type            = BadAlignedFunctorMock<void()>;
        mock_type::behavior        = []() {};
        const auto& performedCalls = mock_type::init();
        auto expectedCalls         = performedCalls;
        // check proper test setup
        CHECK(alignof(mock_type) > sizeof(void*));
        CHECK((performedCalls == expectedCalls));
        {
            mock_type f{};
            ++expectedCalls.defaultConstruction;
            CHECK((performedCalls == expectedCalls));

            auto d1 = delegate_type::create(f);
            ++expectedCalls.copyConstruction;
            ++expectedCalls.newOperator;
            CHECK((performedCalls == expectedCalls));

            d1();
            ++expectedCalls.callOperator;
            CHECK((performedCalls == expectedCalls));
            {
                auto d2{std::move(d1)};
                CHECK((performedCalls == expectedCalls));
                CHECK(!static_cast<bool>(d1));
                CHECK(static_cast<bool>(d2));

                d1();
                CHECK((performedCalls == expectedCalls));

                d1 = delegate_type{};
                d1();
                CHECK((performedCalls == expectedCalls));

                d2();
                ++expectedCalls.callOperator;
                CHECK((performedCalls == expectedCalls));
            }
            ++expectedCalls.destruction;     // destroys d2
            ++expectedCalls.deleteOperator;  // deletes d2
            CHECK((performedCalls == expectedCalls));
        }
        // destruction of d1 doesn't call ~mock_type
        ++expectedCalls.destruction;  // destroys f
        CHECK((performedCalls == expectedCalls));
    }
}

TEST_SUITE_END();

}  // namespace test_rome_delegate
