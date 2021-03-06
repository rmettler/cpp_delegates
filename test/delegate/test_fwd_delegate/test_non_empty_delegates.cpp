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

#include "checks.hpp"
#include "mocks.hpp"

TYPE_TO_STRING(rome::target_is_mandatory);
TYPE_TO_STRING(rome::target_is_expected);
TYPE_TO_STRING(rome::target_is_optional);

TEST_SUITE_BEGIN(
    "header file: rome/delegate.hpp"
    * doctest::description(
        "Create non empty delegates with 'rome::fwd_delegate<...>::create' functions and test:\n"
        " - instance is created as expected\n"
        " - instance is destroyed as expected\n"
        " - 'Ret operator()(Args...)' works as expected for non empty delegates\n"
        " - 'operator bool()' returns true\n"
        " - 'operator==' with nullptr returns false\n"
        " - 'operator!=' with nullptr returns true"));

TEST_CASE_TEMPLATE("rome::fwd_delegate - create with Signature <void(int)> and ", TBehavior,
    rome::target_is_mandatory, rome::target_is_expected, rome::target_is_optional) {
    using TSignature  = void(int);
    using TDelegate   = rome::fwd_delegate<TSignature, TBehavior>;
    auto callBehavior = [](int) {};
    SUBCASE("create from funciton") {
        using TMock                = test_rome_delegate::FunctionMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;

        SUBCASE("and target throwing no exception") {
            {
                static_assert(noexcept(TDelegate::template create<&TMock::mockedCall>()), "");
                auto dgt = TDelegate::template create<&TMock::mockedCall>();
                test_rome_delegate::check(dgt).isNotEmpty();
                dgt(0);
                expectedCalls.callOperator = 1;
                CHECK(expectedCalls == performedCalls);
            }
            CHECK(expectedCalls == performedCalls);
        }
        SUBCASE("and target throwing an exception") {
            using TMock2                = test_rome_delegate::FunctionMock<TSignature, 2>;
            TMock2::behavior            = [](int) { throw 42; };
            const auto& performedCalls2 = TMock2::init();
            auto expectedCalls2         = performedCalls2;
            {
                auto dgt = TDelegate::template create<&TMock2::mockedCall>();
                test_rome_delegate::check(dgt).isNotEmpty();
                CHECK_THROWS_AS(dgt(0), int);
                expectedCalls2.callOperator = 1;
                CHECK(expectedCalls2 == performedCalls2);
            }
            CHECK(expectedCalls == performedCalls);
        }
    }
    SUBCASE("create from non-const method") {
        using TMock                = test_rome_delegate::MethodMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;

        TMock mock;
        {
            static_assert(
                noexcept(TDelegate::template create<TMock, &TMock::mockedCall>(mock)), "");
            auto dgt = TDelegate::template create<TMock, &TMock::mockedCall>(mock);
            test_rome_delegate::check(dgt).isNotEmpty();
            dgt(0);
            expectedCalls.callOperator = 1;
            CHECK(expectedCalls == performedCalls);
        }
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("create from const method") {
        using TMock                = test_rome_delegate::ConstMethodMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;

        const TMock mock;
        {
            static_assert(
                noexcept(TDelegate::template create<TMock, &TMock::mockedCall>(mock)), "");
            auto dgt = TDelegate::template create<TMock, &TMock::mockedCall>(mock);
            test_rome_delegate::check(dgt).isNotEmpty();
            dgt(0);
            expectedCalls.callOperator = 1;
            CHECK(expectedCalls == performedCalls);
        }
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("create from small buffer optimizable functor") {
        using TMock                = test_rome_delegate::SmallFunctorMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        CHECK(sizeof(TMock) <= sizeof(void*));
        CHECK(alignof(TMock) <= alignof(void*));
        CHECK(alignof(TMock) <= sizeof(void*));

        TMock mock;
        expectedCalls.defaultConstruction = 1;
        {
            static_assert(noexcept(TDelegate::create(mock)), "");
            auto dgt                       = TDelegate::create(std::move(mock));
            expectedCalls.moveConstruction = 1;
            CHECK(expectedCalls == performedCalls);
            test_rome_delegate::check(dgt).isNotEmpty();
            dgt(0);
            expectedCalls.callOperator = 1;
            CHECK(expectedCalls == performedCalls);
        }
        expectedCalls.destruction = 1;
        CHECK(expectedCalls == performedCalls);

        struct SmallExceptFunctor {
            SmallExceptFunctor() = default;
            SmallExceptFunctor(SmallExceptFunctor&&) noexcept(false) {
                throw std::bad_alloc{};
            }
            bool operator()(int) {
                return false;
            }
        };
        SmallExceptFunctor mock2;
        static_assert(!noexcept(TDelegate::create(std::move(mock2))), "");
    }
    SUBCASE("create from too big functor for small buffer optimization") {
        using TMock                = test_rome_delegate::BiggerFunctorMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        CHECK(sizeof(TMock) > sizeof(void*));
        CHECK(alignof(TMock) <= alignof(void*));
        CHECK(alignof(TMock) <= sizeof(void*));

        TMock mock;
        expectedCalls.defaultConstruction = 1;
        {
            auto dgt                       = TDelegate::create(mock);
            expectedCalls.copyConstruction = 1;
            expectedCalls.newOperator      = 1;
            CHECK(expectedCalls == performedCalls);
            test_rome_delegate::check(dgt).isNotEmpty();
            dgt(0);
            expectedCalls.callOperator = 1;
            CHECK(expectedCalls == performedCalls);
        }
        expectedCalls.deleteOperator = 1;
        expectedCalls.destruction    = 1;
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("create from too badly aligned functor for small buffer optimization") {
        using TMock                = test_rome_delegate::BadAlignedFunctorMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        CHECK(alignof(TMock) > alignof(void*));
        CHECK(alignof(TMock) > sizeof(void*));

        TMock mock;
        expectedCalls.defaultConstruction = 1;
        {
            auto dgt                       = TDelegate::create(std::move(mock));
            expectedCalls.moveConstruction = 1;
            expectedCalls.newOperator      = 1;
            CHECK(expectedCalls == performedCalls);
            test_rome_delegate::check(dgt).isNotEmpty();
            dgt(0);
            expectedCalls.callOperator = 1;
            CHECK(expectedCalls == performedCalls);
        }
        expectedCalls.deleteOperator = 1;
        expectedCalls.destruction    = 1;
        CHECK(expectedCalls == performedCalls);
    }
}

TEST_SUITE_END();
