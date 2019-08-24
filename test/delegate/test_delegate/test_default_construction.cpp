//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//


#include "test_default_construction.hpp"

#include <doctest.h>

#include <rome/delegate.hpp>
#include <type_traits>

#include "checks.hpp"

// explicit template instantiations
template class ::rome::delegate<void()>;
template class ::rome::delegate<void(int)>;
template class ::rome::delegate<int()>;
template class ::rome::delegate<int(int)>;

enum E : int { e_zero, e_one };
template class ::rome::delegate<void(E)>;
template class ::rome::delegate<E(E)>;

enum class EC : int { zero, one };
template class ::rome::delegate<void(EC)>;
template class ::rome::delegate<EC(EC)>;

union U {
    int i;
};
template class ::rome::delegate<void(U)>;
template class ::rome::delegate<U(U)>;

struct C {
    int i;
    int memberFunction(int) {
        return i;
    }
    int constMemberFunction(int) const {
        return i;
    }
};
template class ::rome::delegate<void(C)>;
template class ::rome::delegate<C(C)>;
template class ::rome::delegate<void(C&)>;
template class ::rome::delegate<C&(C&)>;
template class ::rome::delegate<void(const C&)>;
template class ::rome::delegate<const C&(const C&)>;
template class ::rome::delegate<void(C&&)>;
template class ::rome::delegate<C && (C &&)>;
template class ::rome::delegate<void(C*)>;
template class ::rome::delegate<C*(C*)>;
template class ::rome::delegate<void(const C*)>;
template class ::rome::delegate<const C*(const C*)>;

using TMemberObject = int C::*;
template class ::rome::delegate<void(TMemberObject)>;
template class ::rome::delegate<TMemberObject(TMemberObject)>;

using TMemberFunction = int (C::*)(int);
template class ::rome::delegate<void(TMemberFunction)>;
template class ::rome::delegate<TMemberFunction(TMemberFunction)>;

using TConstMemberFunction = int (C::*)(int) const;
template class ::rome::delegate<void(TConstMemberFunction)>;
template class ::rome::delegate<TConstMemberFunction(TConstMemberFunction)>;

using TArray = int[10];
template class ::rome::delegate<void(TArray)>;

using TArrayRef = int (&)[10];
template class ::rome::delegate<void(TArrayRef)>;
template class ::rome::delegate<TArrayRef(TArrayRef)>;

using TArrayPtr = int (*)[10];
template class ::rome::delegate<void(TArrayPtr)>;
template class ::rome::delegate<TArrayPtr(TArrayPtr)>;

using TFunctionRef = int (&)(int);
template class ::rome::delegate<void(TFunctionRef)>;
template class ::rome::delegate<TFunctionRef(TFunctionRef)>;

using TFunctionPtr = int (*)(int);
template class ::rome::delegate<void(TFunctionPtr)>;
template class ::rome::delegate<TFunctionPtr(TFunctionPtr)>;


namespace test_rome_delegate {
namespace default_construction {

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
        void test_default_construction_for() {
            using rome::delegate;
            delegate<Signature, true> d1;
            checkEmpty(d1);
            delegate<Signature, false> d2;
            checkEmpty(d2);
        }
    }  // namespace

    void test_default_construction() {
        test_default_construction_for<void()>();
        test_default_construction_for<void(int)>();
        test_default_construction_for<int(void)>();
        test_default_construction_for<int(int)>();

        // enum E
        test_default_construction_for<void(E)>();
        test_default_construction_for<E(E)>();

        // enum class EC
        test_default_construction_for<void(EC)>();
        test_default_construction_for<EC(EC)>();

        // union U
        test_default_construction_for<void(U)>();
        test_default_construction_for<U(U)>();

        // class C
        test_default_construction_for<void(C)>();
        test_default_construction_for<C(C)>();
        test_default_construction_for<void(C&)>();
        test_default_construction_for<C&(C&)>();
        test_default_construction_for<void(const C&)>();
        test_default_construction_for<const C&(const C&)>();
        test_default_construction_for<void(C &&)>();
        test_default_construction_for<C && (C &&)>();
        test_default_construction_for<void(C*)>();
        test_default_construction_for<C*(C*)>();
        test_default_construction_for<void(const C*)>();
        test_default_construction_for<const C*(const C*)>();

        using TMemberObject = int C::*;
        test_default_construction_for<void(TMemberObject)>();
        test_default_construction_for<TMemberObject(TMemberObject)>();

        using TMemberFunction = int (C::*)(int);
        test_default_construction_for<void(TMemberFunction)>();
        test_default_construction_for<TMemberFunction(TMemberFunction)>();

        using TConstMemberFunction = int (C::*)(int) const;
        test_default_construction_for<void(TConstMemberFunction)>();
        test_default_construction_for<TConstMemberFunction(TConstMemberFunction)>();

        using TArray = int[10];
        test_default_construction_for<void(TArray)>();

        using TArrayRef = int(&)[10];
        test_default_construction_for<void(TArrayRef)>();
        test_default_construction_for<TArrayRef(TArrayRef)>();

        using TArrayPtr = int(*)[10];
        test_default_construction_for<void(TArrayPtr)>();
        test_default_construction_for<TArrayPtr(TArrayPtr)>();

        using TFunctionRef = int (&)(int);
        test_default_construction_for<void(TFunctionRef)>();
        test_default_construction_for<TFunctionRef(TFunctionRef)>();

        using TFunctionPtr = int (*)(int);
        test_default_construction_for<void(TFunctionPtr)>();
        test_default_construction_for<TFunctionPtr(TFunctionPtr)>();

        // for many different arguments
        test_default_construction_for<add_ptr_ptr_ptr<C>>();
    }

}  // namespace default_construction
}  // namespace test_rome_delegate
