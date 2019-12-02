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
    inline static void callEmptyDelegate(const TDelegate& dgt, Args... args) {
        CHECK_THROWS_WITH_AS(static_cast<void>(dgt(std::forward<Args...>(args...))),
            "rome::bad_delegate_call", rome::bad_delegate_call);
    }
};

template<>
struct helper<rome::target_is_expected> {
    template<typename TDelegate, typename... Args>
    inline static void callEmptyDelegate(const TDelegate& dgt, Args... args) {
        CHECK_THROWS_WITH_AS(static_cast<void>(dgt(std::forward<Args...>(args...))),
            "rome::bad_delegate_call", rome::bad_delegate_call);
    }
};

template<>
struct helper<rome::target_is_optional> {
    template<typename TDelegate, typename... Args>
    inline static void callEmptyDelegate(const TDelegate& dgt, Args... args) {
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

    template<typename Mock>
    struct delegate_creator;

    template<size_t N>
    struct delegate_creator<test_rome_delegate::FunctionMock<TSignature, N>> {
        template<typename T>
        static auto create(T&&) {
            return rome::delegate<TSignature,
                TExpectedBehavior>::template create<&mock_type<N>::mockedCall>();
        }
    };

    template<size_t N>
    struct delegate_creator<test_rome_delegate::MethodMock<TSignature, N>> {
        template<typename T>
        static auto create(T&& mock) {
            return rome::delegate<TSignature, TExpectedBehavior>::template create<mock_type<N>,
                &mock_type<N>::mockedCall>(std::forward<T>(mock));
        }
    };

    template<size_t N>
    struct delegate_creator<test_rome_delegate::ConstMethodMock<TSignature, N>> {
        template<typename T>
        static auto create(T&& mock) {
            return rome::delegate<TSignature, TExpectedBehavior>::template create<mock_type<N>,
                &mock_type<N>::mockedCall>(std::forward<T>(mock));
        }
    };

    template<size_t N>
    struct delegate_creator<test_rome_delegate::SmallFunctorMock<TSignature, N>> {
        template<typename T>
        static auto create(T&& mock) {
            return rome::delegate<TSignature, TExpectedBehavior>::template create(
                std::forward<T>(mock));
        }
    };

    template<size_t N>
    struct delegate_creator<test_rome_delegate::BiggerFunctorMock<TSignature, N>> {
        template<typename T>
        static auto create(T&& mock) {
            return rome::delegate<TSignature, TExpectedBehavior>::template create(
                std::forward<T>(mock));
        }
    };

    template<size_t N>
    struct delegate_creator<test_rome_delegate::BadAlignedFunctorMock<TSignature, N>> {
        template<typename T>
        static auto create(T&& mock) {
            return rome::delegate<TSignature, TExpectedBehavior>::template create(
                std::forward<T>(mock));
        }
    };

    template<size_t N, typename T>
    static auto createFromMock(T&& mock) {
        return delegate_creator<mock_type<N>>::create(std::forward<T>(mock));
    }
};

void mockedBehavior1(int) {
}

bool mockedBehavior2(int i) {
    return i > 0;
}

template<typename... Ts>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

template<typename TSignature, typename TExpectedBehavior, TSignature* mockBehavior>
using function_tests =
    std::tuple<test<TSignature, TExpectedBehavior, mockBehavior, test_rome_delegate::FunctionMock>,
        test<TSignature, TExpectedBehavior, mockBehavior, test_rome_delegate::MethodMock>,
        test<TSignature, TExpectedBehavior, mockBehavior, test_rome_delegate::ConstMethodMock>>;

// TODO: make 2 versions for function style and for functor style!
// TODO:   or adapt the tester/mocks
// TODO: separate delegate creator! -> create_delegate_from_mock<TSignature, TExpectedBehavior, TMock>

// clang-format off
using function_test_vector = tuple_cat_t<
    function_tests<void(int), rome::target_is_mandatory, &mockedBehavior1>,
    function_tests<void(int), rome::target_is_expected,  &mockedBehavior1>,
    function_tests<void(int), rome::target_is_optional,  &mockedBehavior1>,
    function_tests<bool(int), rome::target_is_mandatory, &mockedBehavior2>,
    function_tests<bool(int), rome::target_is_expected,  &mockedBehavior2>
    >;
// clang-format on

TYPE_TO_STRING(void(int));
TYPE_TO_STRING(bool(int));
TYPE_TO_STRING(rome::target_is_mandatory);
TYPE_TO_STRING(rome::target_is_expected);
TYPE_TO_STRING(rome::target_is_optional);

TEST_CASE_TEMPLATE_DEFINE(
    "rome::delegate - move from delegates with function style targets", TTest, function_tests) {
    using TSignature        = typename TTest::signature;
    using TExpectedBehavior = typename TTest::expected_behavior;
    using TMock             = typename TTest::template mock_type<1>;
    TMock::behavior         = TTest::mockBehavior;
    INFO("Signature = " << doctest::detail::type_to_string<TSignature>());
    INFO("ExpectedBehavior = " << doctest::detail::type_to_string<TExpectedBehavior>());
    INFO("Mock = " << TMock::typeToString());

    const auto& performedCalls = TMock::init();
    auto expectedCalls         = performedCalls;
    TMock mock{};
    auto dgt = TTest::template createFromMock<1>(mock);
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
    dgt = TTest::template createFromMock<2>(dummyMock);
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

TEST_CASE_TEMPLATE_APPLY(function_tests, function_test_vector);

TEST_CASE_TEMPLATE("rome::delegate<void(int)> - move ", TExpectedBehavior,
    rome::target_is_mandatory, rome::target_is_expected, rome::target_is_optional) {
    using TSignature  = void(int);
    using TDelegate   = rome::delegate<TSignature, TExpectedBehavior>;
    using TDummyMock  = test_rome_delegate::FunctionMock<TSignature, 0>;
    auto callBehavior = [](int) {};
    /*
        SUBCASE("created from funciton") {
            using TMock                = test_rome_delegate::FunctionMock<TSignature, 1>;
            TMock::behavior            = callBehavior;
            const auto& performedCalls = TMock::init();
            auto expectedCalls         = performedCalls;
            auto dgt                   = TDelegate::template create<&TMock::mockedCall>();
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

            dgt = TDelegate::template create<&TDummyMock::mockedCall>();
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
        SUBCASE("created from non-const method") {
            using TMock                = test_rome_delegate::MethodMock<TSignature, 1>;
            TMock::behavior            = callBehavior;
            const auto& performedCalls = TMock::init();
            auto expectedCalls         = performedCalls;
            TMock mock;
            auto dgt = TDelegate::template create<TMock, &TMock::mockedCall>(mock);
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

            dgt = TDelegate::template create<&TDummyMock::mockedCall>();
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
        SUBCASE("created from const method") {
            using TMock                = test_rome_delegate::ConstMethodMock<TSignature, 1>;
            TMock::behavior            = callBehavior;
            const auto& performedCalls = TMock::init();
            auto expectedCalls         = performedCalls;
            const TMock mock;
            auto dgt = TDelegate::template create<TMock, &TMock::mockedCall>(mock);
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

            dgt = TDelegate::template create<&TDummyMock::mockedCall>();
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
    */
    SUBCASE("created from small buffer optimizable functor") {
        using TMock                = test_rome_delegate::SmallFunctorMock<TSignature, 1>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        expectedCalls.defaultConstruction = 1;
        {
            auto dgt                       = TDelegate::template create(std::move(mock));
            expectedCalls.moveConstruction = 3;
            expectedCalls.destruction      = 2;
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

            dgt = TDelegate::template create<&TDummyMock::mockedCall>();
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
        expectedCalls.destruction = 3;
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("created from too big functor for small buffer optimization") {
        using TMock                = test_rome_delegate::BadAlignedFunctorMock<TSignature, 1>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        expectedCalls.defaultConstruction = 1;
        {
            auto dgt                       = TDelegate::template create(std::move(mock));
            expectedCalls.moveConstruction = 3;
            expectedCalls.newOperator      = 1;
            expectedCalls.destruction      = 2;
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

            dgt = TDelegate::template create<&TDummyMock::mockedCall>();
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
        expectedCalls.destruction    = 3;
        CHECK(expectedCalls == performedCalls);
    }
    SUBCASE("created from too badly aligned functor for small buffer optimization") {
        using TMock                = test_rome_delegate::BadAlignedFunctorMock<TSignature, 1>;
        TMock::behavior            = callBehavior;
        const auto& performedCalls = TMock::init();
        auto expectedCalls         = performedCalls;
        TMock mock;
        expectedCalls.defaultConstruction = 1;
        {
            auto dgt                       = TDelegate::template create(std::move(mock));
            expectedCalls.moveConstruction = 3;
            expectedCalls.newOperator      = 1;
            expectedCalls.destruction      = 2;
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

            dgt = TDelegate::template create<&TDummyMock::mockedCall>();
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
        expectedCalls.destruction    = 3;
        CHECK(expectedCalls == performedCalls);
    }
}

// TODO: rewrite to test vector for assigning empty

TEST_CASE_TEMPLATE("rome::delegate<void(int)> - move from empty delegate", TExpectedBehavior,
    rome::target_is_expected, rome::target_is_optional) {
    using TSignature           = void(int);
    using TDelegate            = rome::delegate<TSignature, TExpectedBehavior>;
    using TDummyMock           = test_rome_delegate::FunctionMock<TSignature, 0>;
    using TMock                = test_rome_delegate::FunctionMock<TSignature, 1>;
    TMock::behavior            = [](int) {};
    const auto& performedCalls = TMock::init();
    auto expectedCalls         = performedCalls;

    auto dgt  = TDelegate{};
    auto dgt2 = std::move(dgt);
    CHECK(expectedCalls == performedCalls);
    test_rome_delegate::checkEmpty(dgt);
    test_rome_delegate::checkEmpty(dgt2);
    helper<TExpectedBehavior>::callEmptyDelegate(dgt, 1);
    CHECK(expectedCalls == performedCalls);
    helper<TExpectedBehavior>::callEmptyDelegate(dgt2, 2);
    CHECK(expectedCalls == performedCalls);

    dgt = TDelegate::template create<&TDummyMock::mockedCall>();
    dgt = std::move(dgt2);
    CHECK(expectedCalls == performedCalls);
    test_rome_delegate::checkEmpty(dgt2);
    test_rome_delegate::checkEmpty(dgt);
    helper<TExpectedBehavior>::callEmptyDelegate(dgt2, 2);
    CHECK(expectedCalls == performedCalls);
    helper<TExpectedBehavior>::callEmptyDelegate(dgt, 1);
    CHECK(expectedCalls == performedCalls);
}

TEST_SUITE_END();
