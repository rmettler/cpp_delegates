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

namespace test_rome_delegate {

struct FunctorCallCounter {
    int defaultConstruction = 0;
    int destruction         = 0;
    int copyConstruction    = 0;
    int moveConstruction    = 0;
    int copyAssignment      = 0;
    int moveAssignment      = 0;
    int callOperator        = 0;
    int newOperator         = 0;
    int deleteOperator      = 0;

    void reset() {
        *this = {};
    }

    bool operator==(const FunctorCallCounter& rhs) const {
        CHECK(defaultConstruction == rhs.defaultConstruction);
        CHECK(destruction == rhs.destruction);
        CHECK(copyConstruction == rhs.copyConstruction);
        CHECK(moveConstruction == rhs.moveConstruction);
        CHECK(copyAssignment == rhs.copyAssignment);
        CHECK(moveAssignment == rhs.moveAssignment);
        CHECK(callOperator == rhs.callOperator);
        CHECK(newOperator == rhs.newOperator);
        CHECK(deleteOperator == rhs.deleteOperator);
        return (defaultConstruction == rhs.defaultConstruction) && (destruction == rhs.destruction)
               && (copyConstruction == rhs.copyConstruction)
               && (moveConstruction == rhs.moveConstruction)
               && (copyAssignment == rhs.copyAssignment) && (moveAssignment == rhs.moveAssignment)
               && (callOperator == rhs.callOperator) && (newOperator == rhs.newOperator)
               && (deleteOperator == rhs.deleteOperator);
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
    static call_counter_type performedCalls;

    static const call_counter_type& init() {
        performedCalls = {};
        return performedCalls;
    }
};
template<typename Derived, typename CallCounter, typename Ret, typename... Args>
typename MockBase<Derived, CallCounter, Ret(Args...)>::mocked_call_type
    MockBase<Derived, CallCounter, Ret(Args...)>::behavior = nullptr;
template<typename Derived, typename CallCounter, typename Ret, typename... Args>
typename MockBase<Derived, CallCounter, Ret(Args...)>::call_counter_type
    MockBase<Derived, CallCounter, Ret(Args...)>::performedCalls = {};


template<typename Signature, size_t N = 0>
class FunctionMock;

template<typename Ret, typename... Args, size_t N>
class FunctionMock<Ret(Args...), N>
    : public MockBase<FunctionMock<Ret(Args...), N>, int, Ret(Args...)> {
    using base     = MockBase<FunctionMock, int, Ret(Args...)>;
    FunctionMock() = delete;

  public:
    using performed_calls_type = typename base::call_counter_type;
    static Ret mockedCall(Args... args) {
        ++base::performedCalls;
        return (*base::behavior)(std::forward<Args>(args)...);
    }
};

template<typename Signature, size_t N = 0>
class MethodMock;

template<typename Ret, typename... Args, size_t N>
class MethodMock<Ret(Args...), N>
    : public MockBase<MethodMock<Ret(Args...), N>, int, Ret(Args...)> {
    using base = MockBase<MethodMock, int, Ret(Args...)>;

  public:
    Ret mockedCall(Args... args) {
        ++base::performedCalls;
        return (*base::behavior)(std::forward<Args>(args)...);
    }
};

template<typename Signature, size_t N = 0>
class ConstMethodMock;

template<typename Ret, typename... Args, size_t N>
class ConstMethodMock<Ret(Args...), N>
    : public MockBase<ConstMethodMock<Ret(Args...), N>, int, Ret(Args...)> {
    using base = MockBase<ConstMethodMock, int, Ret(Args...)>;

  public:
    Ret mockedCall(Args... args) const {
        ++base::performedCalls;
        return (*base::behavior)(std::forward<Args>(args)...);
    }
};

template<typename Signature, size_t N = 0>
class SmallFunctorMock;

template<typename Ret, typename... Args, size_t N>
class SmallFunctorMock<Ret(Args...), N>
    : public MockBase<SmallFunctorMock<Ret(Args...), N>, FunctorCallCounter, Ret(Args...)> {
    using base = MockBase<SmallFunctorMock, FunctorCallCounter, Ret(Args...)>;

    // dummy member to create size
    void* dummy_;

  public:
    // Mocked functions from here!
    SmallFunctorMock() {
        ++base::performedCalls.defaultConstruction;
    }
    ~SmallFunctorMock() {
        ++base::performedCalls.destruction;
    }
    SmallFunctorMock(const SmallFunctorMock&) {
        ++base::performedCalls.copyConstruction;
    }
    SmallFunctorMock(SmallFunctorMock&&) {
        ++base::performedCalls.moveConstruction;
    }
    SmallFunctorMock& operator=(const SmallFunctorMock&) {
        ++base::performedCalls.copyAssignment;
        return *this;
    }
    SmallFunctorMock& operator=(SmallFunctorMock&&) {
        ++base::performedCalls.moveAssignment;
        return *this;
    }
    Ret operator()(Args... args) {
        ++base::performedCalls.callOperator;
        return (*base::behavior)(std::forward<Args>(args)...);
    }
    static void* operator new(std::size_t sz) {
        ++base::performedCalls.newOperator;
        return ::operator new(sz);
    }
    static void operator delete(void* ptr) {
        ++base::performedCalls.deleteOperator;
        ::operator delete(ptr);
    }
};

template<typename Signature, size_t N = 0>
class BiggerFunctorMock;

template<typename Ret, typename... Args, size_t N>
class BiggerFunctorMock<Ret(Args...), N>
    : public MockBase<BiggerFunctorMock<Ret(Args...), N>, FunctorCallCounter, Ret(Args...)> {
    using base = MockBase<BiggerFunctorMock, FunctorCallCounter, Ret(Args...)>;

    // dummy member to create size
    void* dummy1_;
    bool dummy2_;

  public:
    // Mocked functions from here!
    BiggerFunctorMock() {
        ++base::performedCalls.defaultConstruction;
    }
    ~BiggerFunctorMock() {
        ++base::performedCalls.destruction;
    }
    BiggerFunctorMock(const BiggerFunctorMock&) {
        ++base::performedCalls.copyConstruction;
    }
    BiggerFunctorMock(BiggerFunctorMock&&) {
        ++base::performedCalls.moveConstruction;
    }
    BiggerFunctorMock& operator=(const BiggerFunctorMock&) {
        ++base::performedCalls.copyAssignment;
        return *this;
    }
    BiggerFunctorMock& operator=(BiggerFunctorMock&&) {
        ++base::performedCalls.moveAssignment;
        return *this;
    }
    Ret operator()(Args... args) {
        ++base::performedCalls.callOperator;
        return (*base::behavior)(std::forward<Args>(args)...);
    }
    static void* operator new(std::size_t sz) {
        ++base::performedCalls.newOperator;
        return ::operator new(sz);
    }
    static void operator delete(void* ptr) {
        ++base::performedCalls.deleteOperator;
        ::operator delete(ptr);
    }
};

template<typename Signature, size_t N = 0>
class BadAlignedFunctorMock;

template<typename Ret, typename... Args, size_t N>
class BadAlignedFunctorMock<Ret(Args...), N>
    : public MockBase<BadAlignedFunctorMock<Ret(Args...), N>, FunctorCallCounter, Ret(Args...)> {
    using base = MockBase<BadAlignedFunctorMock, FunctorCallCounter, Ret(Args...)>;

    // dummy member to create size and alignment
    alignas(2 * sizeof(void*)) void* dummy_;

  public:
    // Mocked functions from here!
    BadAlignedFunctorMock() {
        ++base::performedCalls.defaultConstruction;
    }
    ~BadAlignedFunctorMock() {
        ++base::performedCalls.destruction;
    }
    BadAlignedFunctorMock(const BadAlignedFunctorMock&) {
        ++base::performedCalls.copyConstruction;
    }
    BadAlignedFunctorMock(BadAlignedFunctorMock&&) {
        ++base::performedCalls.moveConstruction;
    }
    BadAlignedFunctorMock& operator=(const BadAlignedFunctorMock&) {
        ++base::performedCalls.copyAssignment;
        return *this;
    }
    BadAlignedFunctorMock& operator=(BadAlignedFunctorMock&&) {
        ++base::performedCalls.moveAssignment;
        return *this;
    }
    Ret operator()(Args... args) {
        ++base::performedCalls.callOperator;
        return (*base::behavior)(std::forward<Args>(args)...);
    }
    static void* operator new(std::size_t sz) {
        ++base::performedCalls.newOperator;
        return ::operator new(sz);
    }
    static void operator delete(void* ptr) {
        ++base::performedCalls.deleteOperator;
        ::operator delete(ptr);
    }
};

}  // namespace test_rome_delegate