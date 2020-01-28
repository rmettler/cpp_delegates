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
#include <utility>

#include "checks.hpp"
#include "delegate_traits.hpp"
#include "mocks.hpp"

TEST_SUITE_BEGIN("header file: rome/delegate.hpp"
                 * doctest::description("Swap delegates with:\n"
                                        " - 'rome::fwd_delegate<...>::swap(fwd_delegate&)'\n"
                                        " - 'rome::swap(fwd_delegate&, fwd_delegate&)'\n"
                                        "and test that both instances behave as expected with:\n"
                                        " - 'Ret operator()(Args...)'\n"
                                        " - 'operator bool()'\n"
                                        " - 'operator==' with nullptr\n"
                                        " - 'operator!=' with nullptr"));

namespace test = ::test_rome_delegate;

template<typename... Ts>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

template<typename Delegate, delegate_signature<Delegate>* mockBehavior1_,
    delegate_signature<Delegate>* mockBehavior2_, template<typename, size_t> class Mock>
struct Test {
    using delegate_type = Delegate;

    template<size_t N>
    using mock_type = Mock<delegate_signature<Delegate>, N>;

    static constexpr delegate_signature<Delegate>* mockBehavior1 = mockBehavior1_;
    static constexpr delegate_signature<Delegate>* mockBehavior2 = mockBehavior2_;
};

static void mockedBehavior1(int) {
}

// clang-format off
template<template<typename, size_t> class Mock>
using delegates_to_test = std::tuple<
    Test<rome::fwd_delegate<void(int), rome::target_is_mandatory>, &mockedBehavior1,  &mockedBehavior1,  Mock>,
    Test<rome::fwd_delegate<void(int), rome::target_is_expected>,  &mockedBehavior1,  &mockedBehavior1,  Mock>,
    Test<rome::fwd_delegate<void(int), rome::target_is_optional>,  &mockedBehavior1,  &mockedBehavior1,  Mock>
    >;

using tests = tuple_cat_t<
    delegates_to_test<test_rome_delegate::FunctionMock>,
    delegates_to_test<test_rome_delegate::MethodMock>,
    delegates_to_test<test_rome_delegate::ConstMethodMock>,
    delegates_to_test<test_rome_delegate::SmallFunctorMock>,
    delegates_to_test<test_rome_delegate::BiggerFunctorMock>,
    delegates_to_test<test_rome_delegate::BadAlignedFunctorMock>
    >;
// clang-format on

TYPE_TO_STRING(void(int));
TYPE_TO_STRING(bool(int));
TYPE_TO_STRING(rome::target_is_mandatory);
TYPE_TO_STRING(rome::target_is_expected);
TYPE_TO_STRING(rome::target_is_optional);

TEST_CASE_TEMPLATE_DEFINE("rome::fwd_delegate - swap targets", TTest, tests) {
    using Delegate  = typename TTest::delegate_type;
    using Mock1     = typename TTest::template mock_type<1>;
    using Mock2     = test::SmallFunctorMock<delegate_signature<Delegate>, 2>;
    Mock1::behavior = TTest::mockBehavior1;
    Mock2::behavior = TTest::mockBehavior2;
    INFO("Signature = " << doctest::detail::type_to_string<delegate_signature<Delegate>>());
    INFO("Behavior = " << doctest::detail::type_to_string<delegate_behavior<Delegate>>());
    INFO("Mock = " << Mock1::typeToString());

    SUBCASE("swap two non-empty delegates") {
        Mock1 mockedTarget1{};
        Mock2 mockedTarget2{};
        auto dgt1 = test_rome_delegate::create_delegate_from_mock<Delegate, 1>(mockedTarget1);
        auto dgt2 = test_rome_delegate::create_delegate_from_mock<Delegate, 1>(mockedTarget2);
        CHECK(test::check(dgt1).isNotEmpty());
        CHECK(test::check(dgt2).isNotEmpty());
        Mock1::init();
        Mock2::init();
        CHECK(Mock1::expectedCalls == Mock1::performedCalls);
        CHECK(test::check<Mock1>(dgt1).callingItWith(1));
        CHECK(test::check<Mock2>(dgt2).callingItWith(1));
        SUBCASE("swap using fwd_delegate::swap") {
            dgt1.swap(dgt2);
            CHECK(Mock1::expectedCalls == Mock1::performedCalls);
            CHECK(test::check<Mock2>(dgt1).callingItWith(1));
            CHECK(test::check<Mock1>(dgt2).callingItWith(1));
            dgt1.swap(dgt2);
            CHECK(Mock1::expectedCalls == Mock1::performedCalls);
            CHECK(test::check<Mock1>(dgt1).callingItWith(1));
            CHECK(test::check<Mock2>(dgt2).callingItWith(1));
        }
        SUBCASE("swap using global swap") {
            using std::swap;
            swap(dgt1, dgt2);
            CHECK(Mock1::expectedCalls == Mock1::performedCalls);
            CHECK(test::check<Mock2>(dgt1).callingItWith(1));
            CHECK(test::check<Mock1>(dgt2).callingItWith(1));
            swap(dgt1, dgt2);
            CHECK(Mock1::expectedCalls == Mock1::performedCalls);
            CHECK(test::check<Mock1>(dgt1).callingItWith(1));
            CHECK(test::check<Mock2>(dgt2).callingItWith(1));
        }
    }
    SUBCASE("swap non-empty with empty fwd_delegate") {
        Mock1 mockedTarget1{};
        auto dgt2 = test_rome_delegate::create_delegate_from_mock<Delegate, 1>(mockedTarget1);
        auto dgt1 = std::move(dgt2);
        CHECK(test::check(dgt1).isNotEmpty());
        CHECK(test::check(dgt2).isEmpty());
        Mock1::init();
        CHECK(Mock1::expectedCalls == Mock1::performedCalls);
        CHECK(test::check<Mock1>(dgt1).callingItWith(1));
        SUBCASE("swap using fwd_delegate::swap") {
            dgt1.swap(dgt2);
            CHECK(Mock1::expectedCalls == Mock1::performedCalls);
            CHECK(test::check(dgt1).isEmpty());
            CHECK(test::check(dgt2).isNotEmpty());
            CHECK(test::check<Mock1>(dgt2).callingItWith(1));
            dgt1.swap(dgt2);
            CHECK(Mock1::expectedCalls == Mock1::performedCalls);
            CHECK(test::check(dgt1).isNotEmpty());
            CHECK(test::check(dgt2).isEmpty());
            CHECK(test::check<Mock1>(dgt1).callingItWith(1));
        }
        SUBCASE("swap using global swap") {
            using std::swap;
            swap(dgt1, dgt2);
            CHECK(Mock1::expectedCalls == Mock1::performedCalls);
            CHECK(test::check(dgt1).isEmpty());
            CHECK(test::check(dgt2).isNotEmpty());
            CHECK(test::check<Mock1>(dgt2).callingItWith(1));
            swap(dgt1, dgt2);
            CHECK(Mock1::expectedCalls == Mock1::performedCalls);
            CHECK(test::check(dgt1).isNotEmpty());
            CHECK(test::check(dgt2).isEmpty());
            CHECK(test::check<Mock1>(dgt1).callingItWith(1));
        }
    }
    SUBCASE("swap two empty delegates") {
        Mock1 mockedTarget1{};
        auto dgt2  = test_rome_delegate::create_delegate_from_mock<Delegate, 1>(mockedTarget1);
        auto dgt1  = std::move(dgt2);
        auto dummy = std::move(dgt1);
        CHECK(test::check(dgt1).isEmpty());
        CHECK(test::check(dgt2).isEmpty());
        Mock1::init();
        CHECK(Mock1::expectedCalls == Mock1::performedCalls);
        SUBCASE("swap using fwd_delegate::swap") {
            dgt1.swap(dgt2);
            CHECK(Mock1::expectedCalls == Mock1::performedCalls);
            CHECK(test::check(dgt1).isEmpty());
            CHECK(test::check(dgt2).isEmpty());
            dgt1.swap(dgt2);
            CHECK(Mock1::expectedCalls == Mock1::performedCalls);
            CHECK(test::check(dgt1).isEmpty());
            CHECK(test::check(dgt2).isEmpty());
        }
        SUBCASE("swap using global swap") {
            using std::swap;
            swap(dgt1, dgt2);
            CHECK(Mock1::expectedCalls == Mock1::performedCalls);
            CHECK(test::check(dgt1).isEmpty());
            CHECK(test::check(dgt2).isEmpty());
            swap(dgt1, dgt2);
            CHECK(Mock1::expectedCalls == Mock1::performedCalls);
            CHECK(test::check(dgt1).isEmpty());
            CHECK(test::check(dgt2).isEmpty());
        }
    }
}
TEST_CASE_TEMPLATE_APPLY(tests, tests);

TEST_SUITE_END();
