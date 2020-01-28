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

TEST_SUITE_BEGIN(
    "header file: rome/delegate.hpp"
    * doctest::description("Assign nullptr to delegates and test:\n"
                           " - 'Ret operator()(Args...)' works as expected for empty delegates\n"
                           " - 'operator bool()' returns false\n"
                           " - 'operator==' with nullptr returns true\n"
                           " - 'operator!=' with nullptr returns false"));

#define CHECK_CALLING_EMPTY_THROWS(shouldThrow, call)                                              \
    if (shouldThrow) {                                                                             \
        CHECK_THROWS_WITH_AS(call, "rome::bad_delegate_call", rome::bad_delegate_call);            \
    }                                                                                              \
    else {                                                                                         \
        CHECK_NOTHROW(call);                                                                       \
    }

template<bool shouldThrow, typename TDelegate, typename... Args>
static void assign_to_empty_delegate_and_expect_throwing(Args&&... args) {
    {
        TDelegate dgt;
        dgt = nullptr;
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_CALLING_EMPTY_THROWS(shouldThrow, dgt(std::forward<Args...>(args...)));
    }
    {
        TDelegate dgt{};
        dgt = nullptr;
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_CALLING_EMPTY_THROWS(shouldThrow, dgt(std::forward<Args...>(args...)));
    }
    {
        TDelegate dgt(nullptr);
        dgt = nullptr;
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_CALLING_EMPTY_THROWS(shouldThrow, dgt(std::forward<Args...>(args...)));
    }
    {
        TDelegate dgt{nullptr};
        dgt = nullptr;
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_CALLING_EMPTY_THROWS(shouldThrow, dgt(std::forward<Args...>(args...)));
    }
}

TEST_CASE("rome::fwd_delegate - assign nullptr to empty fwd_delegate") {
    assign_to_empty_delegate_and_expect_throwing<false,
        rome::fwd_delegate<void(int), rome::target_is_optional>>(1);
    assign_to_empty_delegate_and_expect_throwing<true,
        rome::fwd_delegate<void(int), rome::target_is_expected>>(2);
}

template<bool shouldThrow, typename TDelegate, typename TSignature, typename... Args>
void assign_to_non_empty_delegate_and_expect_throwing(TSignature* callBehavior, Args... args) {
    {
        INFO("unassigning a function target");
        using TMock                = test_rome_delegate::FunctionMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        auto dgt                   = TDelegate::template create<&TMock::mockedCall>();
        static_cast<void>(dgt(std::forward<Args...>(args...)));
        expectedCalls.callOperator = 1;
        CHECK(expectedCalls == performedCalls);

        dgt = nullptr;
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_CALLING_EMPTY_THROWS(shouldThrow, dgt(std::forward<Args...>(args...)));
        CHECK(expectedCalls == performedCalls);
    }
    {
        INFO("unassigning a non-const method target");
        using TMock                = test_rome_delegate::MethodMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        auto dgt = TDelegate::template create<TMock, &TMock::mockedCall>(mock);
        static_cast<void>(dgt(std::forward<Args...>(args...)));
        expectedCalls.callOperator = 1;
        CHECK(expectedCalls == performedCalls);

        dgt = nullptr;
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_CALLING_EMPTY_THROWS(shouldThrow, dgt(std::forward<Args...>(args...)));
        CHECK(expectedCalls == performedCalls);
    }
    {
        INFO("unassigning a const method target");
        using TMock                = test_rome_delegate::ConstMethodMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        auto dgt = TDelegate::template create<TMock, &TMock::mockedCall>(mock);
        static_cast<void>(dgt(std::forward<Args...>(args...)));
        expectedCalls.callOperator = 1;
        CHECK(expectedCalls == performedCalls);

        dgt = nullptr;
        test_rome_delegate::check(dgt).isEmpty();
        CHECK_CALLING_EMPTY_THROWS(shouldThrow, dgt(std::forward<Args...>(args...)));
        CHECK(expectedCalls == performedCalls);
    }
    {
        INFO("unassigning a small buffer optimizable functor");
        using TMock                = test_rome_delegate::SmallFunctorMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        expectedCalls.defaultConstruction = 1;
        {
            auto dgt                       = TDelegate::create(std::move(mock));
            expectedCalls.moveConstruction = 1;
            static_cast<void>(dgt(std::forward<Args...>(args...)));
            expectedCalls.callOperator = 1;
            CHECK(expectedCalls == performedCalls);

            dgt = nullptr;
            ++expectedCalls.destruction;
            test_rome_delegate::check(dgt).isEmpty();
            CHECK_CALLING_EMPTY_THROWS(shouldThrow, dgt(std::forward<Args...>(args...)));
            CHECK(expectedCalls == performedCalls);
        }
        CHECK(expectedCalls == performedCalls);
    }
    {
        INFO("unassigning a big functor for small buffer optimization");
        using TMock                = test_rome_delegate::BiggerFunctorMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        expectedCalls.defaultConstruction = 1;
        {
            auto dgt                       = TDelegate::create(std::move(mock));
            expectedCalls.moveConstruction = 1;
            expectedCalls.newOperator = 1;
            static_cast<void>(dgt(std::forward<Args...>(args...)));
            expectedCalls.callOperator = 1;
            CHECK(expectedCalls == performedCalls);

            dgt = nullptr;
            ++expectedCalls.deleteOperator;
            ++expectedCalls.destruction;
            test_rome_delegate::check(dgt).isEmpty();
            CHECK_CALLING_EMPTY_THROWS(shouldThrow, dgt(std::forward<Args...>(args...)));
            CHECK(expectedCalls == performedCalls);
        }
        CHECK(expectedCalls == performedCalls);
    }
    {
        INFO("unassigning a too badly aligned functor for small buffer optimization");
        using TMock                = test_rome_delegate::BadAlignedFunctorMock<TSignature>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        expectedCalls.defaultConstruction = 1;
        {
            auto dgt                       = TDelegate::create(std::move(mock));
            expectedCalls.moveConstruction = 1;
            expectedCalls.newOperator = 1;
            static_cast<void>(dgt(std::forward<Args...>(args...)));
            expectedCalls.callOperator = 1;
            CHECK(expectedCalls == performedCalls);

            dgt = nullptr;
            ++expectedCalls.deleteOperator;
            ++expectedCalls.destruction;
            test_rome_delegate::check(dgt).isEmpty();
            CHECK_CALLING_EMPTY_THROWS(shouldThrow, dgt(std::forward<Args...>(args...)));
            CHECK(expectedCalls == performedCalls);
        }
        CHECK(expectedCalls == performedCalls);
    }
}

TEST_CASE("rome::fwd_delegate - assign nullptr to non empty fwd_delegate") {
    {
        using signature     = void(int);
        using fwd_delegate      = rome::fwd_delegate<signature, rome::target_is_optional>;
        signature* behavior = [](int) {};
        assign_to_non_empty_delegate_and_expect_throwing<false, fwd_delegate>(behavior, 1);
    }
    {
        using signature     = void(int);
        using fwd_delegate      = rome::fwd_delegate<signature, rome::target_is_expected>;
        signature* behavior = [](int) {};
        assign_to_non_empty_delegate_and_expect_throwing<true, fwd_delegate>(behavior, 2);
    }
}

TEST_SUITE_END();
