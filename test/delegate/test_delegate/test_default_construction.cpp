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

namespace test_rome_delegate {

TEST_SUITE_BEGIN("header file: rome/delegate.hpp");

namespace {
    template<typename Delegate>
    constexpr bool testDefaultConstruction() {
        static_assert(std::is_nothrow_default_constructible<Delegate>::value, "");
        // TODO add tests
        return true;
    }

    template<typename Signature>
    struct tester;

    template<typename Ret, typename... Args>
    struct tester<Ret(Args...)> {
        template<typename Character>
        using delegate_type = rome::delegate<void(Args...), Character>;
        static constexpr bool test() {
            testDefaultConstruction<delegate_type<rome::target_is_optional>>();
            testDefaultConstruction<delegate_type<rome::target_is_expected>>();
            testDefaultConstruction<delegate_type<rome::target_is_mandatory>>();
            return true;
        }
    };

    template<typename... Args>
    struct tester<void(Args...)> {
        template<typename Character>
        using delegate_type = rome::delegate<void(Args...), Character>;
        static constexpr bool test() {
            testDefaultConstruction<delegate_type<rome::target_is_optional>>();
            testDefaultConstruction<delegate_type<rome::target_is_expected>>();
            static_assert(
                !std::is_default_constructible<delegate_type<rome::target_is_mandatory>>::value,
                "");
            return true;
        }
    };

    template<typename Signature>
    constexpr void test() {
        static_assert(tester<Signature>::test(), "");
    }

    template<typename Signature>
    void test_with_non_void_ret() {
        {
            // TODO: target_is_optional fehlt!
            using T = rome::delegate<Signature, rome::target_is_expected>;
            static_assert(std::is_nothrow_default_constructible<T>::value, "");
            T defaultDgt;
            test_rome_delegate::checkEmpty(defaultDgt);
            T valueInitDgt{};
            test_rome_delegate::checkEmpty(valueInitDgt);
        }
        {
            using T = rome::delegate<Signature, rome::target_is_mandatory>;
            static_assert(!std::is_default_constructible<T>::value, "");
        }
    }

    template<typename Signature>
    void test_with_void_ret() {
        test_with_non_void_ret<Signature>();  // TODO passt nicht! einzeln aufschreiben
        using T = rome::delegate<Signature, rome::target_is_optional>;
        static_assert(std::is_nothrow_default_constructible<T>::value, "");
        T defaultDgt;
        test_rome_delegate::checkEmpty(defaultDgt);
        T valueInitDgt{};
        test_rome_delegate::checkEmpty(valueInitDgt);
    }
}  // namespace

enum E : int { e_zero, e_one };
enum class EC : int { zero, one };
union U {
    int i;
};
struct C {
    int i;
    int memberFunction(int) {
        return i;
    }
    int constMemberFunction(int) const {
        return i;
    }
};

void test_default_construction() {
    test<void()>();
    test_with_void_ret<void()>();
    test_with_void_ret<void(int)>();
    test_with_non_void_ret<int(void)>();
    test_with_non_void_ret<int(int)>();

    // enum E
    test_with_void_ret<void(E)>();
    test_with_non_void_ret<E(E)>();

    // enum class EC
    test_with_void_ret<void(EC)>();
    test_with_non_void_ret<EC(EC)>();

    // union U
    test_with_void_ret<void(U)>();
    test_with_non_void_ret<U(U)>();

    // class C
    test_with_void_ret<void(C)>();
    test_with_non_void_ret<C(C)>();
    test_with_void_ret<void(C&)>();
    test_with_non_void_ret<C&(C&)>();
    test_with_void_ret<void(const C&)>();
    test_with_non_void_ret<const C&(const C&)>();
    test_with_void_ret<void(C &&)>();
    test_with_non_void_ret<C && (C &&)>();
    test_with_void_ret<void(C*)>();
    test_with_non_void_ret<C*(C*)>();
    test_with_void_ret<void(const C*)>();
    test_with_non_void_ret<const C*(const C*)>();

    using TMemberObject = int C::*;
    test_with_void_ret<void(TMemberObject)>();
    test_with_non_void_ret<TMemberObject(TMemberObject)>();

    using TMemberFunction = int (C::*)(int);
    test_with_void_ret<void(TMemberFunction)>();
    test_with_non_void_ret<TMemberFunction(TMemberFunction)>();

    using TConstMemberFunction = int (C::*)(int) const;
    test_with_void_ret<void(TConstMemberFunction)>();
    test_with_non_void_ret<TConstMemberFunction(TConstMemberFunction)>();

    using TArray = int[10];
    test_with_void_ret<void(TArray)>();

    using TArrayRef = int(&)[10];
    test_with_void_ret<void(TArrayRef)>();
    test_with_non_void_ret<TArrayRef(TArrayRef)>();

    using TArrayPtr = int(*)[10];
    test_with_void_ret<void(TArrayPtr)>();
    test_with_non_void_ret<TArrayPtr(TArrayPtr)>();

    using TFunctionRef = int (&)(int);
    test_with_void_ret<void(TFunctionRef)>();
    test_with_non_void_ret<TFunctionRef(TFunctionRef)>();

    using TFunctionPtr = int (*)(int);
    test_with_void_ret<void(TFunctionPtr)>();
    test_with_non_void_ret<TFunctionPtr(TFunctionPtr)>();
}

TEST_CASE("rome::delegate - default construction") {
    test_default_construction();
}

}  // namespace test_rome_delegate

TEST_SUITE_END();
