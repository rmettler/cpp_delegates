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

namespace test = ::test_rome_delegate;

template<typename... Ts>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

template<typename Delegate, delegate_signature<Delegate>* mockBehavior_,
    template<typename, size_t> class Mock>
struct Test {
    using delegate_type = Delegate;

    template<size_t N>
    using mock_type = Mock<delegate_signature<Delegate>, N>;

    static constexpr delegate_signature<Delegate>* mockBehavior = mockBehavior_;
};

void mockedBehavior1(int) {
}

bool mockedBehavior2(int i) {
    return i > 0;
}

// clang-format off
template<template<typename, size_t> class Mock>
using delegates_tested_for_move_from_non_empty = std::tuple<
    Test<rome::delegate<void(int), rome::target_is_mandatory>, &mockedBehavior1, Mock>,
    Test<rome::delegate<void(int), rome::target_is_expected>,  &mockedBehavior1, Mock>,
    Test<rome::delegate<void(int), rome::target_is_optional>,  &mockedBehavior1, Mock>,
    Test<rome::delegate<bool(int), rome::target_is_mandatory>, &mockedBehavior2, Mock>,
    Test<rome::delegate<bool(int), rome::target_is_expected>,  &mockedBehavior2, Mock>
    >;

template<template<typename, size_t> class Mock>
using delegates_tested_for_move_from_empty = std::tuple<
    Test<rome::delegate<void(int), rome::target_is_expected>,  &mockedBehavior1, Mock>,
    Test<rome::delegate<void(int), rome::target_is_optional>,  &mockedBehavior1, Mock>,
    Test<rome::delegate<bool(int), rome::target_is_expected>,  &mockedBehavior2, Mock>
    >;

using function_tests = tuple_cat_t<
    delegates_tested_for_move_from_non_empty<test_rome_delegate::FunctionMock>,
    delegates_tested_for_move_from_non_empty<test_rome_delegate::MethodMock>,
    delegates_tested_for_move_from_non_empty<test_rome_delegate::ConstMethodMock>>;

using small_functor_tests = 
    delegates_tested_for_move_from_non_empty<test_rome_delegate::SmallFunctorMock>;

using big_functor_tests = tuple_cat_t<
    delegates_tested_for_move_from_non_empty<test_rome_delegate::BiggerFunctorMock>,
    delegates_tested_for_move_from_non_empty<test_rome_delegate::BadAlignedFunctorMock>>;

using move_from_empty_tests = 
    delegates_tested_for_move_from_empty<test_rome_delegate::SmallFunctorMock>;
// clang-format on

TYPE_TO_STRING(void(int));
TYPE_TO_STRING(bool(int));
TYPE_TO_STRING(rome::target_is_mandatory);
TYPE_TO_STRING(rome::target_is_expected);
TYPE_TO_STRING(rome::target_is_optional);

TEST_CASE_TEMPLATE_DEFINE(
    "rome::delegate - move from delegates with function style targets", TTest, function_tests) {
    using Delegate            = typename TTest::delegate_type;
    using Mock1               = typename TTest::template mock_type<1>;
    Mock1::behavior           = TTest::mockBehavior;
    INFO("Signature = " << doctest::detail::type_to_string<delegate_signature<Delegate>>());
    INFO("ExpectedBehavior = " << doctest::detail::type_to_string<delegate_behavior<Delegate>>());
    INFO("Mock = " << Mock1::typeToString());

    Mock1::init();
    Mock1 mockedTarget1{};
    auto dgt = test_rome_delegate::create_delegate_from_mock<Delegate, 1>(mockedTarget1);
    CHECK(test::check(dgt).isNotEmpty());
    CHECK(Mock1::expectedCalls == Mock1::performedCalls);

    auto dgt2 = std::move(dgt);
    CHECK(test::check(dgt).isEmpty());
    CHECK(test::check(dgt2).isNotEmpty());
    CHECK(Mock1::expectedCalls == Mock1::performedCalls);

    CHECK(test::check(dgt).callingEmptyDelegateWith(1));
    CHECK(test::check<Mock1>(dgt2).callingItWith(2));

    typename TTest::template mock_type<2> mockedTarget2{};
    dgt = test_rome_delegate::create_delegate_from_mock<Delegate, 2>(mockedTarget2);
    dgt = std::move(dgt2);
    CHECK(test::check(dgt).isNotEmpty());
    CHECK(test::check(dgt2).isEmpty());
    CHECK(Mock1::expectedCalls == Mock1::performedCalls);

    CHECK(test::check(dgt2).callingEmptyDelegateWith(1));
    CHECK(test::check<Mock1>(dgt).callingItWith(2));
}
TEST_CASE_TEMPLATE_APPLY(function_tests, function_tests);

TEST_CASE_TEMPLATE_DEFINE(
    "rome::delegate - move from delegates with small buffer optimizable functor target", TTest,
    small_functor_tests) {
    using Delegate            = typename TTest::delegate_type;
    using Mock1               = typename TTest::template mock_type<1>;
    Mock1::behavior           = TTest::mockBehavior;
    INFO("Signature = " << doctest::detail::type_to_string<delegate_signature<Delegate>>());
    INFO("ExpectedBehavior = " << doctest::detail::type_to_string<delegate_behavior<Delegate>>());
    INFO("Mock = " << Mock1::typeToString());

    Mock1::init();
    Mock1::expectedCalls.defaultConstruction = 1;
    Mock1 mockedTarget1{};
    {
        Mock1::expectedCalls.moveConstruction = 1;
        auto dgt =
            test_rome_delegate::create_delegate_from_mock<Delegate, 1>(std::move(mockedTarget1));
        CHECK(test::check(dgt).isNotEmpty());
        CHECK(Mock1::expectedCalls == Mock1::performedCalls);

        auto dgt2 = std::move(dgt);
        CHECK(test::check(dgt).isEmpty());
        CHECK(test::check(dgt2).isNotEmpty());
        CHECK(Mock1::expectedCalls == Mock1::performedCalls);

        CHECK(test::check(dgt).callingEmptyDelegateWith(1));
        CHECK(test::check<Mock1>(dgt2).callingItWith(2));

        typename TTest::template mock_type<2> mockedTarget2{};
        dgt = test_rome_delegate::create_delegate_from_mock<Delegate, 2>(std::move(mockedTarget2));
        dgt = std::move(dgt2);
        CHECK(test::check(dgt).isNotEmpty());
        CHECK(test::check(dgt2).isEmpty());
        CHECK(Mock1::expectedCalls == Mock1::performedCalls);

        CHECK(test::check(dgt2).callingEmptyDelegateWith(1));
        CHECK(test::check<Mock1>(dgt).callingItWith(2));
    }
    Mock1::expectedCalls.destruction = 1;
    CHECK(Mock1::expectedCalls == Mock1::performedCalls);
}
TEST_CASE_TEMPLATE_APPLY(small_functor_tests, small_functor_tests);

TEST_CASE_TEMPLATE_DEFINE(
    "rome::delegate - move from delegates with non optimizable functor targets", TTest,
    big_functor_tests) {
    using Delegate            = typename TTest::delegate_type;
    using Mock1               = typename TTest::template mock_type<1>;
    Mock1::behavior           = TTest::mockBehavior;
    INFO("Signature = " << doctest::detail::type_to_string<delegate_signature<Delegate>>());
    INFO("ExpectedBehavior = " << doctest::detail::type_to_string<delegate_behavior<Delegate>>());
    INFO("Mock = " << Mock1::typeToString());

    Mock1::init();
    Mock1::expectedCalls.defaultConstruction = 1;
    Mock1 mockedTarget1{};
    {
        Mock1::expectedCalls.moveConstruction = 1;
        Mock1::expectedCalls.newOperator      = 1;
        auto dgt =
            test_rome_delegate::create_delegate_from_mock<Delegate, 1>(std::move(mockedTarget1));
        CHECK(test::check(dgt).isNotEmpty());
        CHECK(Mock1::expectedCalls == Mock1::performedCalls);

        auto dgt2 = std::move(dgt);
        CHECK(test::check(dgt).isEmpty());
        CHECK(test::check(dgt2).isNotEmpty());
        CHECK(Mock1::expectedCalls == Mock1::performedCalls);

        CHECK(test::check(dgt).callingEmptyDelegateWith(1));
        CHECK(test::check<Mock1>(dgt2).callingItWith(2));

        typename TTest::template mock_type<2> mockedTarget2{};
        dgt = test_rome_delegate::create_delegate_from_mock<Delegate, 2>(std::move(mockedTarget2));
        dgt = std::move(dgt2);
        CHECK(test::check(dgt).isNotEmpty());
        CHECK(test::check(dgt2).isEmpty());
        CHECK(Mock1::expectedCalls == Mock1::performedCalls);

        CHECK(test::check(dgt2).callingEmptyDelegateWith(1));
        CHECK(test::check<Mock1>(dgt).callingItWith(2));
    }

    Mock1::expectedCalls.deleteOperator = 1;
    Mock1::expectedCalls.destruction    = 1;
    CHECK(Mock1::expectedCalls == Mock1::performedCalls);
}
TEST_CASE_TEMPLATE_APPLY(big_functor_tests, big_functor_tests);

TEST_CASE_TEMPLATE_DEFINE(
    "rome::delegate<void(int)> - move from empty delegate", TTest, move_from_empty_tests) {
    using Delegate            = typename TTest::delegate_type;
    using Mock1               = typename TTest::template mock_type<1>;
    Mock1::behavior           = TTest::mockBehavior;
    INFO("Signature = " << doctest::detail::type_to_string<delegate_signature<Delegate>>());
    INFO("ExpectedBehavior = " << doctest::detail::type_to_string<delegate_behavior<Delegate>>());
    INFO("Mock = " << Mock1::typeToString());

    Mock1::init();
    {
        auto dgt  = Delegate{};
        auto dgt2 = std::move(dgt);
        CHECK(test::check(dgt).isEmpty());
        CHECK(test::check(dgt2).isEmpty());
        CHECK(Mock1::expectedCalls == Mock1::performedCalls);

        CHECK(test::check(dgt).callingEmptyDelegateWith(1));
        CHECK(test::check(dgt2).callingEmptyDelegateWith(2));

        Mock1::expectedCalls.defaultConstruction = 1;
        Mock1::expectedCalls.destruction         = 1;
        Mock1::expectedCalls.moveConstruction    = 1;
        dgt = test_rome_delegate::create_delegate_from_mock<Delegate, 1>(Mock1{});
        CHECK(test::check(dgt).isNotEmpty());
        CHECK(Mock1::expectedCalls == Mock1::performedCalls);

        Mock1::expectedCalls.destruction = 2;
        dgt = std::move(dgt2);
        CHECK(test::check(dgt2).isEmpty());
        CHECK(test::check(dgt).isEmpty());
        CHECK(Mock1::expectedCalls == Mock1::performedCalls);

        CHECK(test::check(dgt2).callingEmptyDelegateWith(2));
        CHECK(test::check(dgt).callingEmptyDelegateWith(1));
    }
    CHECK(Mock1::expectedCalls == Mock1::performedCalls);
}
TEST_CASE_TEMPLATE_APPLY(move_from_empty_tests, move_from_empty_tests);

TEST_SUITE_END();
