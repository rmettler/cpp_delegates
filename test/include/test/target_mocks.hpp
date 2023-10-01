//
// Project: C++ delegates
//
// Copyright Roger Mettler 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//
// Provides a generic global mock object and different targets that are used in the tests.

#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>

namespace test {

namespace detail {
    template<typename CallSignature>
    struct Mock {
        // NOLINTBEGIN(modernize-use-trailing-return-type)
        MAKE_CONST_MOCK0(defaultConstruct, void());
        MAKE_CONST_MOCK0(copyConstruct, void());
        MAKE_CONST_MOCK0(moveConstruct, void());
        MAKE_CONST_MOCK0(copyAssign, void());
        MAKE_CONST_MOCK0(moveAssign, void());
        MAKE_CONST_MOCK0(destruct, void());
        MAKE_CONST_MOCK1(call, CallSignature);
        MAKE_CONST_MOCK0(new_, void());
        MAKE_CONST_MOCK0(delete_, void());
        // NOLINTEND(modernize-use-trailing-return-type)
    };
}  // namespace detail

// Generic global mock object that is used to define the expected behavior of the mocked targets
// assigned to delegates in the tests.
// All mocked targets below use this mock object.
// Use different numbers 'N' to control multiple mocks at the same time.
template<typename CallSignature, size_t N = 0>
detail::Mock<CallSignature> targetMock{};

namespace detail {
    template<typename Signature>
    struct function_signature_impl;

    template<typename Ret, typename Arg>
    struct function_signature_impl<Ret(Arg)> {
        using return_type   = Ret;
        using argument_type = Arg;
    };

    template<typename Signature>
    using return_type = typename function_signature_impl<Signature>::return_type;

    template<typename Signature>
    using argument_type = typename function_signature_impl<Signature>::argument_type;
}  // namespace detail


// A free function target that is mocked using 'targetMock<...>'.
template<typename CallSignature, size_t N = 0>
auto mockedFunction(detail::argument_type<CallSignature> arg)
    -> detail::return_type<CallSignature> {
    return targetMock<CallSignature, N>.call(arg);
}


// Provides non-static const and non-const member functions.
// The member functions are mocked using 'targetMock<...>'.
template<typename CallSignature, size_t N = 0>
struct TargetClass {
    auto mockedMethod(detail::argument_type<CallSignature> arg)
        -> detail::return_type<CallSignature> {
        return targetMock<CallSignature, N>.call(arg);
    }

    auto mockedConstMethod(detail::argument_type<CallSignature> arg) const
        -> detail::return_type<CallSignature> {
        return targetMock<CallSignature, N>.call(arg);
    }
};

// The object to use for non-static non-const member function targets.
// It is mocked using 'targetMock<...>'.
template<typename CallSignature, size_t N = 0>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TargetClass<CallSignature, N> targetObject{};

// The object to use for non-static const member function targets.
// It is mocked using 'targetMock<...>'.
template<typename CallSignature, size_t N = 0>
const TargetClass<CallSignature, N> targetConstObject{};


namespace detail {
    template<typename CallSignature, size_t N>
    struct FunctorBase {
        FunctorBase() {
            targetMock<CallSignature, N>.defaultConstruct();
        }
        ~FunctorBase() {
            targetMock<CallSignature, N>.destruct();
        }
        FunctorBase(const FunctorBase&) {
            targetMock<CallSignature, N>.copyConstruct();
        }
        FunctorBase(FunctorBase&&) noexcept {
            targetMock<CallSignature, N>.moveConstruct();
        }
        auto operator=(const FunctorBase&) -> FunctorBase& {  // NOLINT(cert-oop54-cpp)
            targetMock<CallSignature, N>.copyAssign();
            return *this;
        }
        auto operator=(FunctorBase&&) noexcept -> FunctorBase& {
            targetMock<CallSignature, N>.moveAssign();
            return *this;
        }
        auto operator()(detail::argument_type<CallSignature> arg) const
            -> detail::return_type<CallSignature> {
            return targetMock<CallSignature, N>.call(arg);
        }
        static auto operator new(std::size_t sz) -> void* {
            targetMock<CallSignature, N>.new_();
            return ::operator new(sz);
        }
        static void operator delete(void* ptr) {
            targetMock<CallSignature, N>.delete_();
            ::operator delete(ptr);
        }
    };
}  // namespace detail

// The class for a functor target that is small buffer optimizable within a delegate (no heap
// allocation needed).
// It is mocked using 'targetMock<...>'.
template<typename CallSignature, size_t N = 0>
class BufferOptimizableFunctor : public detail::FunctorBase<CallSignature, N> {
    void* dummy_ = nullptr;  // dummy member to create size
};
static_assert(sizeof(BufferOptimizableFunctor<void(int)>) == sizeof(void*), "");

// The class for a functor target that is too big for small buffer optimization within a delegate
// (heap allocation needed).
// It is mocked using 'targetMock<...>'.
template<typename CallSignature, size_t N = 0>
class TooBigFunctor : public detail::FunctorBase<CallSignature, N> {
    void* dummy0_ = nullptr;  // dummy member to create size
    bool dummy1_  = {};
};
static_assert(sizeof(TooBigFunctor<void(int)>) > sizeof(void*), "");

// The class for a functor target with an alignment that cannot be small buffer optimized within a
// delegate (heap allocation needed).
// It is mocked using 'targetMock<...>'.
template<typename CallSignature, size_t N = 0>
class BadAlignedFunctor : public detail::FunctorBase<CallSignature, N> {
    alignas(alignof(void*) * 2) bool dummy_ = {};  // dummy member to create size
};
static_assert(alignof(BadAlignedFunctor<void(int)>) > alignof(void*), "");

}  // namespace test