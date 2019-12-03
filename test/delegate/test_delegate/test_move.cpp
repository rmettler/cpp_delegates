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
#include <tuple>
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

template<typename ExpectedBehavior>
struct helper;

template<>
struct helper<rome::target_is_mandatory> {
    template<typename TDelegate, typename... Args>
    inline static void callEmptyDelegate(const TDelegate& dgt, Args&&... args) {
        CHECK_THROWS_WITH_AS(static_cast<void>(dgt(std::forward<Args...>(args...))),
            "rome::bad_delegate_call", rome::bad_delegate_call);
    }
};

template<>
struct helper<rome::target_is_expected> {
    template<typename TDelegate, typename... Args>
    inline static void callEmptyDelegate(const TDelegate& dgt, Args&&... args) {
        CHECK_THROWS_WITH_AS(static_cast<void>(dgt(std::forward<Args...>(args...))),
            "rome::bad_delegate_call", rome::bad_delegate_call);
    }
};

template<>
struct helper<rome::target_is_optional> {
    template<typename TDelegate, typename... Args>
    inline static void callEmptyDelegate(const TDelegate& dgt, Args&&... args) {
        static_cast<void>(dgt(std::forward<Args...>(args...)));
    }
};

template<typename TSignature, typename TExpectedBehavior, TSignature* mockBehavior_,
    template<typename, size_t> class TMock>
struct test {
    using signature         = TSignature;
    using expected_behavior = TExpectedBehavior;

    template<size_t N>
    using mock_type = TMock<TSignature, N>;

    static constexpr TSignature* mockBehavior = mockBehavior_;
};

void mockedBehavior1(int) {
}

bool mockedBehavior2(int i) {
    return i > 0;
}

template<typename... Ts>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

template<typename TSignature, typename TExpectedBehavior, TSignature* mockBehavior>
using function_test_series =
    std::tuple<test<TSignature, TExpectedBehavior, mockBehavior, test_rome_delegate::FunctionMock>,
        test<TSignature, TExpectedBehavior, mockBehavior, test_rome_delegate::MethodMock>,
        test<TSignature, TExpectedBehavior, mockBehavior, test_rome_delegate::ConstMethodMock>>;

template<typename TSignature, typename TExpectedBehavior, TSignature* mockBehavior>
using small_functor_test_series = std::tuple<
    test<TSignature, TExpectedBehavior, mockBehavior, test_rome_delegate::SmallFunctorMock>>;

template<typename TSignature, typename TExpectedBehavior, TSignature* mockBehavior>
using big_functor_test_series = std::tuple<
    test<TSignature, TExpectedBehavior, mockBehavior, test_rome_delegate::BiggerFunctorMock>,
    test<TSignature, TExpectedBehavior, mockBehavior, test_rome_delegate::BadAlignedFunctorMock>>;

// clang-format off
template<template<typename TSignature, typename, TSignature*> class TestSeries>
using tested_delegates_for_move_from_non_empty = tuple_cat_t<
    TestSeries<void(int), rome::target_is_mandatory, &mockedBehavior1>,
    TestSeries<void(int), rome::target_is_expected,  &mockedBehavior1>,
    TestSeries<void(int), rome::target_is_optional,  &mockedBehavior1>,
    TestSeries<bool(int), rome::target_is_mandatory, &mockedBehavior2>,
    TestSeries<bool(int), rome::target_is_expected,  &mockedBehavior2>
    >;

template<template<typename TSignature, typename, TSignature*> class TestSeries>
using tested_delegates_for_move_from_empty = tuple_cat_t<
    TestSeries<void(int), rome::target_is_expected,  &mockedBehavior1>,
    TestSeries<void(int), rome::target_is_optional,  &mockedBehavior1>,
    TestSeries<bool(int), rome::target_is_expected,  &mockedBehavior2>
    >;
// clang-format on

using function_tests      = tested_delegates_for_move_from_non_empty<function_test_series>;
using small_functor_tests = tested_delegates_for_move_from_non_empty<small_functor_test_series>;
using big_functor_tests   = tested_delegates_for_move_from_non_empty<big_functor_test_series>;

using move_from_empty_tests = tested_delegates_for_move_from_empty<small_functor_test_series>;


TYPE_TO_STRING(void(int));
TYPE_TO_STRING(bool(int));
TYPE_TO_STRING(rome::target_is_mandatory);
TYPE_TO_STRING(rome::target_is_expected);
TYPE_TO_STRING(rome::target_is_optional);

TEST_CASE_TEMPLATE_DEFINE(
    "rome::delegate - move from delegates with function style targets", TTest, function_tests) {
    using TSignature           = typename TTest::signature;
    using TExpectedBehavior    = typename TTest::expected_behavior;
    using TDelegate            = rome::delegate<TSignature, TExpectedBehavior>;
    using TMock                = typename TTest::template mock_type<1>;
    TMock::behavior            = TTest::mockBehavior;
    const auto& performedCalls = TMock::init();
    auto expectedCalls         = performedCalls;
    INFO("Signature = " << doctest::detail::type_to_string<TSignature>());
    INFO("ExpectedBehavior = " << doctest::detail::type_to_string<TExpectedBehavior>());
    INFO("Mock = " << TMock::typeToString());

    TMock mock{};
    auto dgt = test_rome_delegate::create_delegate_from_mock<TDelegate, 1>(mock);
    test_rome_delegate::checkNotEmpty(dgt);
    CHECK(expectedCalls == performedCalls);

    auto dgt2 = std::move(dgt);
    CHECK(expectedCalls == performedCalls);
    test_rome_delegate::checkEmpty(dgt);
    test_rome_delegate::checkNotEmpty(dgt2);
    helper<TExpectedBehavior>::callEmptyDelegate(dgt, 1);
    CHECK(expectedCalls == performedCalls);
    static_cast<void>(dgt2(2));
    expectedCalls = 1;
    CHECK(expectedCalls == performedCalls);

    typename TTest::template mock_type<2> dummyMock{};
    dgt = test_rome_delegate::create_delegate_from_mock<TDelegate, 2>(dummyMock);
    dgt = std::move(dgt2);
    CHECK(expectedCalls == performedCalls);
    test_rome_delegate::checkEmpty(dgt2);
    test_rome_delegate::checkNotEmpty(dgt);
    helper<TExpectedBehavior>::callEmptyDelegate(dgt2, 1);
    CHECK(expectedCalls == performedCalls);
    static_cast<void>(dgt(2));
    expectedCalls = 2;
    CHECK(expectedCalls == performedCalls);
}
TEST_CASE_TEMPLATE_APPLY(function_tests, function_tests);

TEST_CASE_TEMPLATE_DEFINE(
    "rome::delegate - move from delegates with small buffer optimizable functor target", TTest,
    small_functor_tests) {
    using TSignature           = typename TTest::signature;
    using TExpectedBehavior    = typename TTest::expected_behavior;
    using TDelegate            = rome::delegate<TSignature, TExpectedBehavior>;
    using TMock                = typename TTest::template mock_type<1>;
    TMock::behavior            = TTest::mockBehavior;
    const auto& performedCalls = TMock::init();
    auto expectedCalls         = performedCalls;
    INFO("Signature = " << doctest::detail::type_to_string<TSignature>());
    INFO("ExpectedBehavior = " << doctest::detail::type_to_string<TExpectedBehavior>());
    INFO("Mock = " << TMock::typeToString());

    TMock mock{};
    expectedCalls.defaultConstruction = 1;
    {
        auto dgt = test_rome_delegate::create_delegate_from_mock<TDelegate, 1>(std::move(mock));
        expectedCalls.moveConstruction = 1;
        test_rome_delegate::checkNotEmpty(dgt);
        CHECK(expectedCalls == performedCalls);

        auto dgt2 = std::move(dgt);
        CHECK(expectedCalls == performedCalls);
        test_rome_delegate::checkEmpty(dgt);
        test_rome_delegate::checkNotEmpty(dgt2);
        helper<TExpectedBehavior>::callEmptyDelegate(dgt, 1);
        CHECK(expectedCalls == performedCalls);
        static_cast<void>(dgt2(2));
        expectedCalls.callOperator = 1;
        CHECK(expectedCalls == performedCalls);

        typename TTest::template mock_type<2> dummyMock{};
        dgt = test_rome_delegate::create_delegate_from_mock<TDelegate, 2>(std::move(dummyMock));
        dgt = std::move(dgt2);
        CHECK(expectedCalls == performedCalls);
        test_rome_delegate::checkEmpty(dgt2);
        test_rome_delegate::checkNotEmpty(dgt);
        helper<TExpectedBehavior>::callEmptyDelegate(dgt2, 1);
        CHECK(expectedCalls == performedCalls);
        static_cast<void>(dgt(2));
        expectedCalls.callOperator = 2;
        CHECK(expectedCalls == performedCalls);
    }
    expectedCalls.destruction = 1;
    CHECK(expectedCalls == performedCalls);
}
TEST_CASE_TEMPLATE_APPLY(small_functor_tests, small_functor_tests);

TEST_CASE_TEMPLATE_DEFINE(
    "rome::delegate - move from delegates with non optimizable functor targets", TTest,
    big_functor_tests) {
    using TSignature           = typename TTest::signature;
    using TExpectedBehavior    = typename TTest::expected_behavior;
    using TDelegate            = rome::delegate<TSignature, TExpectedBehavior>;
    using TMock                = typename TTest::template mock_type<1>;
    TMock::behavior            = TTest::mockBehavior;
    const auto& performedCalls = TMock::init();
    auto expectedCalls         = performedCalls;
    INFO("Signature = " << doctest::detail::type_to_string<TSignature>());
    INFO("ExpectedBehavior = " << doctest::detail::type_to_string<TExpectedBehavior>());
    INFO("Mock = " << TMock::typeToString());

    TMock mock{};
    expectedCalls.defaultConstruction = 1;
    {
        auto dgt = test_rome_delegate::create_delegate_from_mock<TDelegate, 1>(std::move(mock));
        expectedCalls.moveConstruction = 1;
        expectedCalls.newOperator      = 1;
        test_rome_delegate::checkNotEmpty(dgt);
        CHECK(expectedCalls == performedCalls);

        auto dgt2 = std::move(dgt);
        CHECK(expectedCalls == performedCalls);
        test_rome_delegate::checkEmpty(dgt);
        test_rome_delegate::checkNotEmpty(dgt2);
        helper<TExpectedBehavior>::callEmptyDelegate(dgt, 1);
        CHECK(expectedCalls == performedCalls);
        static_cast<void>(dgt2(2));
        expectedCalls.callOperator = 1;
        CHECK(expectedCalls == performedCalls);

        typename TTest::template mock_type<2> dummyMock{};
        dgt = test_rome_delegate::create_delegate_from_mock<TDelegate, 2>(std::move(dummyMock));
        dgt = std::move(dgt2);
        CHECK(expectedCalls == performedCalls);
        test_rome_delegate::checkEmpty(dgt2);
        test_rome_delegate::checkNotEmpty(dgt);
        helper<TExpectedBehavior>::callEmptyDelegate(dgt2, 1);
        CHECK(expectedCalls == performedCalls);
        static_cast<void>(dgt(2));
        expectedCalls.callOperator = 2;
        CHECK(expectedCalls == performedCalls);
    }
    expectedCalls.deleteOperator = 1;
    expectedCalls.destruction    = 1;
    CHECK(expectedCalls == performedCalls);
}
TEST_CASE_TEMPLATE_APPLY(big_functor_tests, big_functor_tests);

TEST_CASE_TEMPLATE_DEFINE(
    "rome::delegate<void(int)> - move from empty delegate", TTest, move_from_empty_tests) {
    using TSignature           = typename TTest::signature;
    using TExpectedBehavior    = typename TTest::expected_behavior;
    using TDelegate            = rome::delegate<TSignature, TExpectedBehavior>;
    using TMock                = typename TTest::template mock_type<1>;
    TMock::behavior            = TTest::mockBehavior;
    const auto& performedCalls = TMock::init();
    auto expectedCalls         = performedCalls;
    INFO("Signature = " << doctest::detail::type_to_string<TSignature>());
    INFO("ExpectedBehavior = " << doctest::detail::type_to_string<TExpectedBehavior>());
    INFO("Mock = " << TMock::typeToString());

    {
        auto dgt  = TDelegate{};
        auto dgt2 = std::move(dgt);
        test_rome_delegate::checkEmpty(dgt);
        test_rome_delegate::checkEmpty(dgt2);
        helper<TExpectedBehavior>::callEmptyDelegate(dgt, 1);
        helper<TExpectedBehavior>::callEmptyDelegate(dgt2, 2);

        dgt = test_rome_delegate::create_delegate_from_mock<TDelegate, 1>(TMock{});
        test_rome_delegate::checkNotEmpty(dgt);
        expectedCalls.defaultConstruction = 1;
        expectedCalls.destruction         = 1;
        expectedCalls.moveConstruction    = 1;
        CHECK(expectedCalls == performedCalls);

        dgt = std::move(dgt2);
        test_rome_delegate::checkEmpty(dgt2);
        test_rome_delegate::checkEmpty(dgt);
        helper<TExpectedBehavior>::callEmptyDelegate(dgt2, 2);
        helper<TExpectedBehavior>::callEmptyDelegate(dgt, 1);
        expectedCalls.destruction = 2;
        CHECK(expectedCalls == performedCalls);
    }
    CHECK(expectedCalls == performedCalls);
}
TEST_CASE_TEMPLATE_APPLY(move_from_empty_tests, move_from_empty_tests);

TEST_SUITE_END();
