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
    // helpers to create a whole lot of function argument combinations of
    // values, pointers, references and cv-modifiers
    template<typename... T>
    using add_ref = int(T..., T&..., T&&...);

    template<typename... T>
    using add_cv = add_ref<T..., T const..., T volatile..., T const volatile...>;

    template<typename... T>
    using add_ptr = add_cv<T..., T*..., T const*..., T volatile*..., T const volatile*...>;

    template<typename... T>
    using add_ptr_ptr = add_ptr<T..., T*..., T const*..., T volatile*..., T const volatile*...>;

    template<typename... T>
    using add_ptr_ptr_ptr =
        add_ptr_ptr<T..., T*..., T const*..., T volatile*..., T const volatile*...>;

    template<typename Signature>
    void test_with_non_void() {
        {
            using T = rome::delegate<Signature, rome::tgt_dyn_req>;
            static_assert(std::is_nothrow_default_constructible<T>::value, "");
            T d;
            test_rome_delegate::checkEmpty(d);
        }
        {
            using T = rome::delegate<Signature, rome::tgt_stat_req>;
            static_assert(!std::is_default_constructible<T>::value, "");
        }
    }

    template<typename Signature>
    void test_with_void_ret() {
        test_with_non_void<Signature>();
        using T = rome::delegate<Signature, rome::tgt_opt>;
        static_assert(std::is_nothrow_default_constructible<T>::value, "");
        T d;
        test_rome_delegate::checkEmpty(d);
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
    test_with_void_ret<void()>();
    test_with_void_ret<void(int)>();
    test_with_non_void<int(void)>();
    test_with_non_void<int(int)>();

    // enum E
    test_with_void_ret<void(E)>();
    test_with_non_void<E(E)>();

    // enum class EC
    test_with_void_ret<void(EC)>();
    test_with_non_void<EC(EC)>();

    // union U
    test_with_void_ret<void(U)>();
    test_with_non_void<U(U)>();

    // class C
    test_with_void_ret<void(C)>();
    test_with_non_void<C(C)>();
    test_with_void_ret<void(C&)>();
    test_with_non_void<C&(C&)>();
    test_with_void_ret<void(const C&)>();
    test_with_non_void<const C&(const C&)>();
    test_with_void_ret<void(C &&)>();
    test_with_non_void<C && (C &&)>();
    test_with_void_ret<void(C*)>();
    test_with_non_void<C*(C*)>();
    test_with_void_ret<void(const C*)>();
    test_with_non_void<const C*(const C*)>();

    using TMemberObject = int C::*;
    test_with_void_ret<void(TMemberObject)>();
    test_with_non_void<TMemberObject(TMemberObject)>();

    using TMemberFunction = int (C::*)(int);
    test_with_void_ret<void(TMemberFunction)>();
    test_with_non_void<TMemberFunction(TMemberFunction)>();

    using TConstMemberFunction = int (C::*)(int) const;
    test_with_void_ret<void(TConstMemberFunction)>();
    test_with_non_void<TConstMemberFunction(TConstMemberFunction)>();

    using TArray = int[10];
    test_with_void_ret<void(TArray)>();

    using TArrayRef = int(&)[10];
    test_with_void_ret<void(TArrayRef)>();
    test_with_non_void<TArrayRef(TArrayRef)>();

    using TArrayPtr = int(*)[10];
    test_with_void_ret<void(TArrayPtr)>();
    test_with_non_void<TArrayPtr(TArrayPtr)>();

    using TFunctionRef = int (&)(int);
    test_with_void_ret<void(TFunctionRef)>();
    test_with_non_void<TFunctionRef(TFunctionRef)>();

    using TFunctionPtr = int (*)(int);
    test_with_void_ret<void(TFunctionPtr)>();
    test_with_non_void<TFunctionPtr(TFunctionPtr)>();

    // for many different arguments
    test_with_non_void<add_ptr_ptr_ptr<C>>();
}

TEST_CASE("rome::delegate - default construction") {
    test_default_construction();
}

}  // namespace test_rome_delegate

TEST_SUITE_END();
