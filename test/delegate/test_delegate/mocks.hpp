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

    bool operator==(const FunctorCallCounter& rhs) {
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

template<typename Signature>
class FunctionMock;

template<typename Ret, typename... Args>
class FunctionMock<Ret(Args...)> {
  public:
    using mock_call_type = Ret (*)(Args...);
    static mock_call_type mockCall;
    static int callCounter;

    static Ret static_function(Args... args) {
        ++callCounter;
        return (*mockCall)(args...);
    }
};
template<typename Ret, typename... Args>
typename FunctionMock<Ret(Args...)>::mock_call_type FunctionMock<Ret(Args...)>::mockCall = nullptr;
template<typename Ret, typename... Args>
int FunctionMock<Ret(Args...)>::callCounter = 0;

template<typename Signature>
class MemberFunctionMock;

template<typename Ret, typename... Args>
class MemberFunctionMock<Ret(Args...)> {
    using mock_call_type = Ret (*)(Args...);
    const mock_call_type mockCall_;

  public:
    static int callCounter;

    // used to create the mock during test setup
    template<typename T>
    MemberFunctionMock(T mockCall) : mockCall_{mockCall} {
    }

    Ret member_function(Args... args) {
        ++callCounter;
        return (*mockCall_)(args...);
    }
};
template<typename Ret, typename... Args>
int MemberFunctionMock<Ret(Args...)>::callCounter = {};

template<typename Signature>
class ConstMemberFunctionMock;

template<typename Ret, typename... Args>
class ConstMemberFunctionMock<Ret(Args...)> {
    using mock_call_type = Ret (*)(Args...);
    const mock_call_type mockCall_;

  public:
    static int callCounter;

    // used to create the mock during test setup
    template<typename T>
    ConstMemberFunctionMock(T mockCall) : mockCall_{mockCall} {
    }

    Ret const_member_function(Args... args) const {
        ++callCounter;
        return (*mockCall_)(args...);
    }
};
template<typename Ret, typename... Args>
int ConstMemberFunctionMock<Ret(Args...)>::callCounter = {};

template<typename Signature>
class SmallFunctorMock;

template<typename Ret, typename... Args>
class SmallFunctorMock<Ret(Args...)> {
    Ret (*mockCall_)(Args...) = nullptr;

  public:
    static FunctorCallCounter callCounter;

    // used to create the mock during test setup
    template<typename T>
    SmallFunctorMock(T mockCall) : mockCall_{mockCall} {
    }

    // Mocked functions from here!
    SmallFunctorMock() {
        ++callCounter.defaultConstruction;
    }
    ~SmallFunctorMock() {
        ++callCounter.destruction;
    }
    SmallFunctorMock(const SmallFunctorMock& orig) : mockCall_{orig.mockCall_} {
        ++callCounter.copyConstruction;
    }
    SmallFunctorMock(SmallFunctorMock&& orig) : mockCall_{orig.mockCall_} {
        ++callCounter.moveConstruction;
    }
    SmallFunctorMock& operator=(const SmallFunctorMock& rhs) {
        ++callCounter.copyAssignment;
        mockCall_ = rhs.mockCall_;
        return *this;
    }
    SmallFunctorMock& operator=(SmallFunctorMock&& rhs) {
        ++callCounter.moveAssignment;
        mockCall_ = rhs.mockCall_;
        return *this;
    }
    Ret operator()(Args... args) {
        ++callCounter.callOperator;
        return (*mockCall_)(args...);
    }
    static void* operator new(std::size_t sz) {
        ++callCounter.newOperator;
        return ::operator new(sz);
    }
    static void operator delete(void* ptr) {
        ++callCounter.deleteOperator;
        ::operator delete(ptr);
    }
};
template<typename Ret, typename... Args>
FunctorCallCounter SmallFunctorMock<Ret(Args...)>::callCounter = {};

template<typename Signature>
class BiggerFunctorMock;

template<typename Ret, typename... Args>
class BiggerFunctorMock<Ret(Args...)> {
    Ret (*mockCall_)(Args...) = nullptr;
    const FunctorCallCounter& callCounter_ =
        callCounter;  // just a dummy address to add another member

  public:
    static FunctorCallCounter callCounter;

    // used to create the mock during test setup
    template<typename T>
    BiggerFunctorMock(T mockCall) : mockCall_{mockCall} {
    }

    // Mocked functions from here!
    BiggerFunctorMock() {
        ++callCounter.defaultConstruction;
    }
    ~BiggerFunctorMock() {
        ++callCounter.destruction;
    }
    BiggerFunctorMock(const BiggerFunctorMock& orig) : mockCall_{orig.mockCall_} {
        ++callCounter.copyConstruction;
    }
    BiggerFunctorMock(BiggerFunctorMock&& orig) : mockCall_{orig.mockCall_} {
        ++callCounter.moveConstruction;
    }
    BiggerFunctorMock& operator=(const BiggerFunctorMock& rhs) {
        ++callCounter.copyAssignment;
        mockCall_ = rhs.mockCall_;
        return *this;
    }
    BiggerFunctorMock& operator=(BiggerFunctorMock&& rhs) {
        ++callCounter.moveAssignment;
        mockCall_ = rhs.mockCall_;
        return *this;
    }
    Ret operator()(Args... args) {
        ++callCounter.callOperator;
        return (*mockCall_)(args...);
    }
    static void* operator new(std::size_t sz) {
        ++callCounter.newOperator;
        return ::operator new(sz);
    }
    static void operator delete(void* ptr) {
        ++callCounter.deleteOperator;
        ::operator delete(ptr);
    }
};
template<typename Ret, typename... Args>
FunctorCallCounter BiggerFunctorMock<Ret(Args...)>::callCounter = {};

template<typename Signature>
class BadAlignedFunctorMock;

template<typename Ret, typename... Args>
class BadAlignedFunctorMock<Ret(Args...)> {
    alignas(2 * sizeof(void*)) Ret (*mockCall_)(Args...) = nullptr;

  public:
    static FunctorCallCounter callCounter;

    // used to create the mock during test setup
    template<typename T>
    BadAlignedFunctorMock(T mockCall) : mockCall_{mockCall} {
    }

    // Mocked functions from here!
    BadAlignedFunctorMock() {
        ++callCounter.defaultConstruction;
    }
    ~BadAlignedFunctorMock() {
        ++callCounter.destruction;
    }
    BadAlignedFunctorMock(const BadAlignedFunctorMock& orig) : mockCall_{orig.mockCall_} {
        ++callCounter.copyConstruction;
    }
    BadAlignedFunctorMock(BadAlignedFunctorMock&& orig) : mockCall_{orig.mockCall_} {
        ++callCounter.moveConstruction;
    }
    BadAlignedFunctorMock& operator=(const BadAlignedFunctorMock& rhs) {
        ++callCounter.copyAssignment;
        mockCall_ = rhs.mockCall_;
        return *this;
    }
    BadAlignedFunctorMock& operator=(BadAlignedFunctorMock&& rhs) {
        ++callCounter.moveAssignment;
        mockCall_ = rhs.mockCall_;
        return *this;
    }
    Ret operator()(Args... args) {
        ++callCounter.callOperator;
        return (*mockCall_)(args...);
    }
    static void* operator new(std::size_t sz) {
        ++callCounter.newOperator;
        return ::operator new(sz);
    }
    static void operator delete(void* ptr) {
        ++callCounter.deleteOperator;
        ::operator delete(ptr);
    }
};
template<typename Ret, typename... Args>
FunctorCallCounter BadAlignedFunctorMock<Ret(Args...)>::callCounter = {};

}  // namespace test_rome_delegate