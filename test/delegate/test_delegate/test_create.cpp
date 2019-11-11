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

TEST_SUITE_BEGIN("header file: rome/delegate.hpp");

TYPE_TO_STRING(rome::target_is_mandatory);
TYPE_TO_STRING(rome::target_is_expected);
TYPE_TO_STRING(rome::target_is_optional);

TEST_CASE_TEMPLATE("rome::delegate - create with Signature <void(int)> and ", TExpectedBehavior,
    rome::target_is_mandatory, rome::target_is_expected, rome::target_is_optional) {
    using TSignature  = void(int);
    using TDelegate   = rome::delegate<TSignature, TExpectedBehavior>;
    auto callBehavior = [](int) {};
    SUBCASE("create from funciton") {
        using TMock                = test_rome_delegate::FunctionMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;

        static_assert(noexcept(TDelegate::template create<&TMock::mockedCall>()), "");
        SUBCASE("as non constexpr delegate") {
            auto dgt = TDelegate::template create<&TMock::mockedCall>();
            test_rome_delegate::checkNotEmpty(dgt);
            dgt(0);
            expectedCalls = 1;
            CHECK(expectedCalls == performedCalls);
        }
        SUBCASE("as constexpr delegate") {
            constexpr auto dgt = TDelegate::template create<&TMock::mockedCall>();
            ROME_DELEGATE_CHECK_NOT_EMPTY(dgt);
            dgt(0);
            expectedCalls = 1;
            CHECK(expectedCalls == performedCalls);
        }
    }
    SUBCASE("create from non-const method") {
        using TMock                = test_rome_delegate::MethodMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;

        static_assert(noexcept(TDelegate::template create<TMock, &TMock::mockedCall>(mock)), "");
        auto dgt = TDelegate::template create<TMock, &TMock::mockedCall>(mock);
        test_rome_delegate::checkNotEmpty(dgt);
        dgt(0);
        expectedCalls = 1;
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("create from const method") {
        using TMock                = test_rome_delegate::ConstMethodMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        const TMock mock;

        static_assert(noexcept(TDelegate::template create<TMock, &TMock::mockedCall>(mock)), "");
        auto dgt = TDelegate::template create<TMock, &TMock::mockedCall>(mock);
        test_rome_delegate::checkNotEmpty(dgt);
        dgt(0);
        expectedCalls = 1;
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("create from small buffer optimizable functor") {
        using TMock                = test_rome_delegate::SmallFunctorMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        expectedCalls.defaultConstruction = 1;

        static_assert(noexcept(TDelegate::create(mock)), "");
        auto dgt                       = TDelegate::create(mock);
        expectedCalls.moveConstruction = 1;
        CHECK(expectedCalls == performedCalls);
        test_rome_delegate::checkNotEmpty(dgt);
        dgt(0);
        expectedCalls.callOperator = 1;
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("create from too big functor for small buffer optimization") {
        using TMock                = test_rome_delegate::BiggerFunctorMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        expectedCalls.defaultConstruction = 1;

        static_assert(noexcept(TDelegate::create(mock)), "");
        auto dgt                       = TDelegate::create(mock);
        expectedCalls.newOperator      = 1;
        expectedCalls.moveConstruction = 1;
        CHECK(expectedCalls == performedCalls);
        test_rome_delegate::checkNotEmpty(dgt);
        dgt(0);
        expectedCalls.callOperator = 1;
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("create from too badly aligned functor for small buffer optimization") {
        using TMock                = test_rome_delegate::BiggerFunctorMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        expectedCalls.defaultConstruction = 1;

        static_assert(noexcept(TDelegate::create(mock)), "");
        auto dgt                       = TDelegate::create(mock);
        expectedCalls.newOperator      = 1;
        expectedCalls.moveConstruction = 1;
        CHECK(expectedCalls == performedCalls);
        test_rome_delegate::checkNotEmpty(dgt);
        dgt(0);
        expectedCalls.callOperator = 1;
        CHECK(expectedCalls == performedCalls);
    }
}

TEST_CASE_TEMPLATE("rome::delegate - create with Signature <void(int)> and ", TExpectedBehavior,
    rome::target_is_mandatory, rome::target_is_expected) {
    using TSignature  = bool(int);
    using TDelegate   = rome::delegate<TSignature, TExpectedBehavior>;
    auto callBehavior = [](int i) { return i > 0; };
    SUBCASE("create from funciton") {
        using TMock                = test_rome_delegate::FunctionMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        SUBCASE("as non constexpr delegate") {
            auto dgt = TDelegate::template create<&TMock::mockedCall>();
            test_rome_delegate::checkNotEmpty(dgt);
            CHECK(dgt(1) == true);
            CHECK(dgt(-1) == false);
            expectedCalls = 2;
            CHECK(expectedCalls == performedCalls);
        }
        SUBCASE("as constexpr delegate") {
            constexpr auto dgt = TDelegate::template create<&TMock::mockedCall>();
            ROME_DELEGATE_CHECK_NOT_EMPTY(dgt);
            CHECK(dgt(1) == true);
            CHECK(dgt(-1) == false);
            expectedCalls = 2;
            CHECK(expectedCalls == performedCalls);
        }
    }
    SUBCASE("create from non-const method") {
        using TMock                = test_rome_delegate::MethodMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;

        auto dgt = TDelegate::template create<TMock, &TMock::mockedCall>(mock);
        test_rome_delegate::checkNotEmpty(dgt);
        CHECK(dgt(1) == true);
        CHECK(dgt(-1) == false);
        expectedCalls = 2;
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("create from const method") {
        using TMock                = test_rome_delegate::ConstMethodMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        const TMock mock;

        auto dgt = TDelegate::template create<TMock, &TMock::mockedCall>(mock);
        test_rome_delegate::checkNotEmpty(dgt);
        CHECK(dgt(1) == true);
        CHECK(dgt(-1) == false);
        expectedCalls = 2;
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("create from small buffer optimizable functor") {
        using TMock                = test_rome_delegate::SmallFunctorMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        expectedCalls.defaultConstruction = 1;

        auto dgt                       = TDelegate::create(mock);
        expectedCalls.moveConstruction = 1;
        CHECK(expectedCalls == performedCalls);
        test_rome_delegate::checkNotEmpty(dgt);
        CHECK(dgt(1) == true);
        CHECK(dgt(-1) == false);
        expectedCalls.callOperator = 2;
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("create from too big functor for small buffer optimization") {
        using TMock                = test_rome_delegate::BiggerFunctorMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        expectedCalls.defaultConstruction = 1;

        static_assert(noexcept(TDelegate::create(mock)), "");
        auto dgt                       = TDelegate::create(mock);
        expectedCalls.newOperator      = 1;
        expectedCalls.moveConstruction = 1;
        CHECK(expectedCalls == performedCalls);
        test_rome_delegate::checkNotEmpty(dgt);
        CHECK(dgt(1) == true);
        CHECK(dgt(-1) == false);
        expectedCalls.callOperator = 2;
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("create from too badly aligned functor for small buffer optimization") {
        using TMock                = test_rome_delegate::BiggerFunctorMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        expectedCalls.defaultConstruction = 1;

        static_assert(noexcept(TDelegate::create(mock)), "");
        auto dgt                       = TDelegate::create(mock);
        expectedCalls.newOperator      = 1;
        expectedCalls.moveConstruction = 1;
        CHECK(expectedCalls == performedCalls);
        test_rome_delegate::checkNotEmpty(dgt);
        CHECK(dgt(1) == true);
        CHECK(dgt(-1) == false);
        expectedCalls.callOperator = 2;
        CHECK(expectedCalls == performedCalls);
    }
}

TEST_SUITE_END();
