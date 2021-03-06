//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <doctest.h>

#include <functional>
#include <memory>

#include "rome/delegate.hpp"

namespace test_rome_delegate {
namespace detail {
    struct CallOperatorCounter {
        int callOperator = 0;

        bool operator==(const CallOperatorCounter& rhs) const {
            CHECK(callOperator == rhs.callOperator);
            return (callOperator == rhs.callOperator);
        }
    };

    struct FunctorOperationsCounter {
        int defaultConstruction = 0;
        int destruction         = 0;
        int copyConstruction    = 0;
        int moveConstruction    = 0;
        int copyAssignment      = 0;
        int moveAssignment      = 0;
        int callOperator        = 0;
        int newOperator         = 0;
        int deleteOperator      = 0;

        bool operator==(const FunctorOperationsCounter& rhs) const {
            CHECK(defaultConstruction == rhs.defaultConstruction);
            CHECK(destruction == rhs.destruction);
            CHECK(copyConstruction == rhs.copyConstruction);
            CHECK(moveConstruction == rhs.moveConstruction);
            CHECK(copyAssignment == rhs.copyAssignment);
            CHECK(moveAssignment == rhs.moveAssignment);
            CHECK(callOperator == rhs.callOperator);
            CHECK(newOperator == rhs.newOperator);
            CHECK(deleteOperator == rhs.deleteOperator);
            return (defaultConstruction == rhs.defaultConstruction)
                   && (destruction == rhs.destruction) && (copyConstruction == rhs.copyConstruction)
                   && (moveConstruction == rhs.moveConstruction)
                   && (copyAssignment == rhs.copyAssignment)
                   && (moveAssignment == rhs.moveAssignment) && (callOperator == rhs.callOperator)
                   && (newOperator == rhs.newOperator) && (deleteOperator == rhs.deleteOperator);
        }
    };

    template<typename Derived, typename CallCounter, typename Signature>
    class MockBase;

    template<typename Derived, typename CallCounter, typename Ret, typename... Args>
    class MockBase<Derived, CallCounter, Ret(Args...)> {
      public:
        using mocked_call_type = Ret (*)(Args...);
        static mocked_call_type behavior;
        using call_counter_type = CallCounter;
        static call_counter_type expectedCalls;
        static call_counter_type performedCalls;

        static const call_counter_type& init() {
            expectedCalls  = {};
            performedCalls = {};
            return performedCalls;
        }
    };
    template<typename Derived, typename CallCounter, typename Ret, typename... Args>
    typename MockBase<Derived, CallCounter, Ret(Args...)>::mocked_call_type
        MockBase<Derived, CallCounter, Ret(Args...)>::behavior = nullptr;
    template<typename Derived, typename CallCounter, typename Ret, typename... Args>
    typename MockBase<Derived, CallCounter, Ret(Args...)>::call_counter_type
        MockBase<Derived, CallCounter, Ret(Args...)>::expectedCalls = {};
    template<typename Derived, typename CallCounter, typename Ret, typename... Args>
    typename MockBase<Derived, CallCounter, Ret(Args...)>::call_counter_type
        MockBase<Derived, CallCounter, Ret(Args...)>::performedCalls = {};
}  // namespace detail

template<typename Signature, size_t N = 0>
class FunctionMock;

template<typename Ret, typename... Args, size_t N>
class FunctionMock<Ret(Args...), N> : public detail::MockBase<FunctionMock<Ret(Args...), N>,
                                          detail::CallOperatorCounter, Ret(Args...)> {
    using base = detail::MockBase<FunctionMock, detail::CallOperatorCounter, Ret(Args...)>;

  public:
    using performed_calls_type = typename base::call_counter_type;
    static Ret mockedCall(Args... args) {
        ++base::performedCalls.callOperator;
        return (*base::behavior)(std::forward<Args>(args)...);
    }
    static const char* typeToString() {
        return "<FunctionMock>";
    }
};

template<typename Signature, size_t N = 0>
class MethodMock;

template<typename Ret, typename... Args, size_t N>
class MethodMock<Ret(Args...), N> : public detail::MockBase<MethodMock<Ret(Args...), N>,
                                        detail::CallOperatorCounter, Ret(Args...)> {
    using base = detail::MockBase<MethodMock, detail::CallOperatorCounter, Ret(Args...)>;

  public:
    Ret mockedCall(Args... args) {
        ++base::performedCalls.callOperator;
        return (*base::behavior)(std::forward<Args>(args)...);
    }
    static const char* typeToString() {
        return "<MethodMock>";
    }
};

template<typename Signature, size_t N = 0>
class ConstMethodMock;

template<typename Ret, typename... Args, size_t N>
class ConstMethodMock<Ret(Args...), N> : public detail::MockBase<ConstMethodMock<Ret(Args...), N>,
                                             detail::CallOperatorCounter, Ret(Args...)> {
    using base = detail::MockBase<ConstMethodMock, detail::CallOperatorCounter, Ret(Args...)>;

  public:
    Ret mockedCall(Args... args) const {
        ++base::performedCalls.callOperator;
        return (*base::behavior)(std::forward<Args>(args)...);
    }
    static const char* typeToString() {
        return "<ConstMethodMock>";
    }
};

template<typename Signature, size_t N = 0>
class SmallFunctorMock;

template<typename Ret, typename... Args, size_t N>
class SmallFunctorMock<Ret(Args...), N> : public detail::MockBase<SmallFunctorMock<Ret(Args...), N>,
                                              detail::FunctorOperationsCounter, Ret(Args...)> {
    using base = detail::MockBase<SmallFunctorMock, detail::FunctorOperationsCounter, Ret(Args...)>;

    // dummy member to create size
    void* dummy_;

  public:
    // Mocked functions from here!
    SmallFunctorMock() noexcept {
        ++base::performedCalls.defaultConstruction;
    }
    ~SmallFunctorMock() noexcept {
        ++base::performedCalls.destruction;
    }
    SmallFunctorMock(const SmallFunctorMock&) noexcept {
        ++base::performedCalls.copyConstruction;
    }
    SmallFunctorMock(SmallFunctorMock&&) noexcept {
        ++base::performedCalls.moveConstruction;
    }
    SmallFunctorMock& operator=(const SmallFunctorMock&) noexcept {
        ++base::performedCalls.copyAssignment;
        return *this;
    }
    SmallFunctorMock& operator=(SmallFunctorMock&&) noexcept {
        ++base::performedCalls.moveAssignment;
        return *this;
    }
    Ret operator()(Args... args) noexcept {
        ++base::performedCalls.callOperator;
        return (*base::behavior)(std::forward<Args>(args)...);
    }
    static void* operator new(std::size_t sz) noexcept {
        ++base::performedCalls.newOperator;
        return ::operator new(sz);
    }
    static void operator delete(void* ptr) noexcept {
        ++base::performedCalls.deleteOperator;
        ::operator delete(ptr);
    }
    static const char* typeToString() {
        return "<SmallFunctorMock>";
    }
};

DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wpadded")

template<typename Signature, size_t N = 0>
class BiggerFunctorMock;

template<typename Ret, typename... Args, size_t N>
class BiggerFunctorMock<Ret(Args...), N>
    : public detail::MockBase<BiggerFunctorMock<Ret(Args...), N>, detail::FunctorOperationsCounter,
          Ret(Args...)> {
    using base =
        detail::MockBase<BiggerFunctorMock, detail::FunctorOperationsCounter, Ret(Args...)>;

    // dummy members to create size
    void* dummy1_;
    bool dummy2_;

  public:
    // Mocked functions from here!
    BiggerFunctorMock() noexcept {
        ++base::performedCalls.defaultConstruction;
    }
    ~BiggerFunctorMock() noexcept {
        ++base::performedCalls.destruction;
    }
    BiggerFunctorMock(const BiggerFunctorMock&) noexcept {
        ++base::performedCalls.copyConstruction;
    }
    BiggerFunctorMock(BiggerFunctorMock&&) noexcept {
        ++base::performedCalls.moveConstruction;
    }
    BiggerFunctorMock& operator=(const BiggerFunctorMock&) noexcept {
        ++base::performedCalls.copyAssignment;
        return *this;
    }
    BiggerFunctorMock& operator=(BiggerFunctorMock&&) noexcept {
        ++base::performedCalls.moveAssignment;
        return *this;
    }
    Ret operator()(Args... args) noexcept {
        ++base::performedCalls.callOperator;
        return (*base::behavior)(std::forward<Args>(args)...);
    }
    static void* operator new(std::size_t sz) noexcept {
        ++base::performedCalls.newOperator;
        return ::operator new(sz);
    }
    static void operator delete(void* ptr) noexcept {
        ++base::performedCalls.deleteOperator;
        ::operator delete(ptr);
    }
    static const char* typeToString() {
        return "<BiggerFunctorMock>";
    }
};

template<typename Signature, size_t N = 0>
class BadAlignedFunctorMock;

template<typename Ret, typename... Args, size_t N>
class BadAlignedFunctorMock<Ret(Args...), N>
    : public detail::MockBase<BadAlignedFunctorMock<Ret(Args...), N>,
          detail::FunctorOperationsCounter, Ret(Args...)> {
    using base =
        detail::MockBase<BadAlignedFunctorMock, detail::FunctorOperationsCounter, Ret(Args...)>;

    // dummy member to create size and alignment
    alignas(2 * sizeof(void*)) void* dummy_;

  public:
    // Mocked functions from here!
    BadAlignedFunctorMock() noexcept {
        ++base::performedCalls.defaultConstruction;
    }
    ~BadAlignedFunctorMock() noexcept {
        ++base::performedCalls.destruction;
    }
    BadAlignedFunctorMock(const BadAlignedFunctorMock&) noexcept {
        ++base::performedCalls.copyConstruction;
    }
    BadAlignedFunctorMock(BadAlignedFunctorMock&&) noexcept {
        ++base::performedCalls.moveConstruction;
    }
    BadAlignedFunctorMock& operator=(const BadAlignedFunctorMock&) noexcept {
        ++base::performedCalls.copyAssignment;
        return *this;
    }
    BadAlignedFunctorMock& operator=(BadAlignedFunctorMock&&) noexcept {
        ++base::performedCalls.moveAssignment;
        return *this;
    }
    Ret operator()(Args... args) noexcept {
        ++base::performedCalls.callOperator;
        return (*base::behavior)(std::forward<Args>(args)...);
    }
    static void* operator new(std::size_t sz) noexcept {
        ++base::performedCalls.newOperator;
        return ::operator new(sz);
    }
    static void operator delete(void* ptr) noexcept {
        ++base::performedCalls.deleteOperator;
        ::operator delete(ptr);
    }
    static const char* typeToString() {
        return "<BadAlignedFunctorMock>";
    }
};

DOCTEST_CLANG_SUPPRESS_WARNING_POP

namespace detail {
    template<typename Delegate, typename Mock>
    struct delegate_creator;

    template<typename Delegate, typename Signature, size_t N>
    struct delegate_creator<Delegate, FunctionMock<Signature, N>> {
        using TMock = FunctionMock<Signature, N>;
        static auto create(TMock&) {
            return Delegate::template create<&TMock::mockedCall>();
        }
    };

    template<typename Delegate, typename Signature, size_t N>
    struct delegate_creator<Delegate, MethodMock<Signature, N>> {
        using TMock = MethodMock<Signature, N>;
        static auto create(TMock& mock) {
            return Delegate::template create<TMock, &TMock::mockedCall>(mock);
        }
    };

    template<typename Delegate, typename Signature, size_t N>
    struct delegate_creator<Delegate, ConstMethodMock<Signature, N>> {
        using TMock = ConstMethodMock<Signature, N>;
        static auto create(const TMock& mock) {
            return Delegate::template create<TMock, &TMock::mockedCall>(mock);
        }
    };

    template<typename Delegate, typename Signature, size_t N>
    struct delegate_creator<Delegate, SmallFunctorMock<Signature, N>> {
        template<typename T>
        static auto create(T&& mock) {
            return Delegate::template create(std::forward<T>(mock));
        }
    };

    template<typename Delegate, typename Signature, size_t N>
    struct delegate_creator<Delegate, BiggerFunctorMock<Signature, N>> {
        template<typename T>
        static auto create(T&& mock) {
            return Delegate::template create(std::forward<T>(mock));
        }
    };

    template<typename Delegate, typename Signature, size_t N>
    struct delegate_creator<Delegate, BadAlignedFunctorMock<Signature, N>> {
        template<typename T>
        static auto create(T&& mock) {
            return Delegate::template create(std::forward<T>(mock));
        }
    };
}  // namespace detail

template<typename Delegate, size_t N, typename Mock>
static auto create_delegate_from_mock(Mock&& mock) {
    return detail::delegate_creator<Delegate, std::decay_t<Mock>>::create(std::forward<Mock>(mock));
}

}  // namespace test_rome_delegate
