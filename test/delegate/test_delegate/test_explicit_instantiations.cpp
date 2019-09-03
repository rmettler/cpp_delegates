//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <rome/delegate.hpp>

#include "checks.hpp"

// explicit template instantiations
template class ::rome::delegate<void(), rome::tgt_dyn_req>;
template class ::rome::delegate<void(), rome::tgt_stat_req>;
template class ::rome::delegate<void(), rome::tgt_opt>;
template class ::rome::delegate<void(int), rome::tgt_dyn_req>;
template class ::rome::delegate<void(int), rome::tgt_stat_req>;
template class ::rome::delegate<void(int), rome::tgt_opt>;
template class ::rome::delegate<int(), rome::tgt_dyn_req>;
template class ::rome::delegate<int(), rome::tgt_stat_req>;
static_assert(
    test_rome_delegate::is_target_req_error_produced<::rome::delegate<int(), rome::tgt_opt>>, "");
template class ::rome::delegate<int(int), rome::tgt_dyn_req>;
template class ::rome::delegate<int(int), rome::tgt_stat_req>;
static_assert(
    test_rome_delegate::is_target_req_error_produced<::rome::delegate<int(int), rome::tgt_opt>>,
    "");

enum E : int { e_zero, e_one };
template class ::rome::delegate<void(E), rome::tgt_dyn_req>;
template class ::rome::delegate<void(E), rome::tgt_stat_req>;
template class ::rome::delegate<void(E), rome::tgt_opt>;
template class ::rome::delegate<E(E), rome::tgt_dyn_req>;
template class ::rome::delegate<E(E), rome::tgt_stat_req>;
static_assert(
    test_rome_delegate::is_target_req_error_produced<::rome::delegate<E(E), rome::tgt_opt>>, "");

enum class EC : int { zero, one };
template class ::rome::delegate<void(EC), rome::tgt_dyn_req>;
template class ::rome::delegate<void(EC), rome::tgt_stat_req>;
template class ::rome::delegate<void(EC), rome::tgt_opt>;
template class ::rome::delegate<EC(EC), rome::tgt_dyn_req>;
template class ::rome::delegate<EC(EC), rome::tgt_stat_req>;
static_assert(
    test_rome_delegate::is_target_req_error_produced<::rome::delegate<EC(EC), rome::tgt_opt>>, "");

union U {
    int i;
};
template class ::rome::delegate<void(U), rome::tgt_dyn_req>;
template class ::rome::delegate<void(U), rome::tgt_stat_req>;
template class ::rome::delegate<void(U), rome::tgt_opt>;
template class ::rome::delegate<U(U), rome::tgt_dyn_req>;
template class ::rome::delegate<U(U), rome::tgt_stat_req>;
static_assert(
    test_rome_delegate::is_target_req_error_produced<::rome::delegate<U(U), rome::tgt_opt>>, "");

struct C {
    int i;
    int memberFunction(int) {
        return i;
    }
    int constMemberFunction(int) const {
        return i;
    }
};
template class ::rome::delegate<void(C), rome::tgt_dyn_req>;
template class ::rome::delegate<void(C), rome::tgt_stat_req>;
template class ::rome::delegate<void(C), rome::tgt_opt>;
template class ::rome::delegate<C(C), rome::tgt_dyn_req>;
template class ::rome::delegate<C(C), rome::tgt_stat_req>;
static_assert(
    test_rome_delegate::is_target_req_error_produced<::rome::delegate<C(C), rome::tgt_opt>>, "");
template class ::rome::delegate<void(C&), rome::tgt_dyn_req>;
template class ::rome::delegate<void(C&), rome::tgt_stat_req>;
template class ::rome::delegate<void(C&), rome::tgt_opt>;
template class ::rome::delegate<C&(C&), rome::tgt_dyn_req>;
template class ::rome::delegate<C&(C&), rome::tgt_stat_req>;
static_assert(
    test_rome_delegate::is_target_req_error_produced<::rome::delegate<C&(C&), rome::tgt_opt>>, "");
template class ::rome::delegate<void(const C&), rome::tgt_dyn_req>;
template class ::rome::delegate<void(const C&), rome::tgt_stat_req>;
template class ::rome::delegate<void(const C&), rome::tgt_opt>;
template class ::rome::delegate<const C&(const C&), rome::tgt_dyn_req>;
template class ::rome::delegate<const C&(const C&), rome::tgt_stat_req>;
static_assert(test_rome_delegate::is_target_req_error_produced<
                  ::rome::delegate<const C&(const C&), rome::tgt_opt>>,
    "");
template class ::rome::delegate<void(C&&), rome::tgt_dyn_req>;
template class ::rome::delegate<void(C&&), rome::tgt_stat_req>;
template class ::rome::delegate<void(C&&), rome::tgt_opt>;
template class ::rome::delegate<C && (C &&), rome::tgt_dyn_req>;
template class ::rome::delegate<C && (C &&), rome::tgt_stat_req>;
static_assert(
    test_rome_delegate::is_target_req_error_produced<::rome::delegate<C && (C &&), rome::tgt_opt>>,
    "");
template class ::rome::delegate<void(C*), rome::tgt_dyn_req>;
template class ::rome::delegate<void(C*), rome::tgt_stat_req>;
template class ::rome::delegate<void(C*), rome::tgt_opt>;
template class ::rome::delegate<C*(C*), rome::tgt_dyn_req>;
template class ::rome::delegate<C*(C*), rome::tgt_stat_req>;
static_assert(
    test_rome_delegate::is_target_req_error_produced<::rome::delegate<C*(C*), rome::tgt_opt>>, "");
template class ::rome::delegate<void(const C*), rome::tgt_dyn_req>;
template class ::rome::delegate<void(const C*), rome::tgt_stat_req>;
template class ::rome::delegate<void(const C*), rome::tgt_opt>;
template class ::rome::delegate<const C*(const C*), rome::tgt_dyn_req>;
template class ::rome::delegate<const C*(const C*), rome::tgt_stat_req>;
static_assert(test_rome_delegate::is_target_req_error_produced<
                  ::rome::delegate<const C*(const C*), rome::tgt_opt>>,
    "");

using TMemberObject = int C::*;
template class ::rome::delegate<void(TMemberObject), rome::tgt_dyn_req>;
template class ::rome::delegate<void(TMemberObject), rome::tgt_stat_req>;
template class ::rome::delegate<void(TMemberObject), rome::tgt_opt>;
template class ::rome::delegate<TMemberObject(TMemberObject), rome::tgt_dyn_req>;
template class ::rome::delegate<TMemberObject(TMemberObject), rome::tgt_stat_req>;
static_assert(test_rome_delegate::is_target_req_error_produced<
                  ::rome::delegate<TMemberObject(TMemberObject), rome::tgt_opt>>,
    "");

using TMemberFunction = int (C::*)(int);
template class ::rome::delegate<void(TMemberFunction), rome::tgt_dyn_req>;
template class ::rome::delegate<void(TMemberFunction), rome::tgt_stat_req>;
template class ::rome::delegate<void(TMemberFunction), rome::tgt_opt>;
template class ::rome::delegate<TMemberFunction(TMemberFunction), rome::tgt_dyn_req>;
template class ::rome::delegate<TMemberFunction(TMemberFunction), rome::tgt_stat_req>;
static_assert(test_rome_delegate::is_target_req_error_produced<
                  ::rome::delegate<TMemberFunction(TMemberFunction), rome::tgt_opt>>,
    "");

using TConstMemberFunction = int (C::*)(int) const;
template class ::rome::delegate<void(TConstMemberFunction), rome::tgt_dyn_req>;
template class ::rome::delegate<void(TConstMemberFunction), rome::tgt_stat_req>;
template class ::rome::delegate<void(TConstMemberFunction), rome::tgt_opt>;
template class ::rome::delegate<TConstMemberFunction(TConstMemberFunction), rome::tgt_dyn_req>;
template class ::rome::delegate<TConstMemberFunction(TConstMemberFunction), rome::tgt_stat_req>;
static_assert(test_rome_delegate::is_target_req_error_produced<
                  ::rome::delegate<TConstMemberFunction(TConstMemberFunction), rome::tgt_opt>>,
    "");

using TArray = int[10];
template class ::rome::delegate<void(TArray), rome::tgt_dyn_req>;
template class ::rome::delegate<void(TArray), rome::tgt_stat_req>;
template class ::rome::delegate<void(TArray), rome::tgt_opt>;

using TArrayRef = int (&)[10];
template class ::rome::delegate<void(TArrayRef), rome::tgt_dyn_req>;
template class ::rome::delegate<void(TArrayRef), rome::tgt_stat_req>;
template class ::rome::delegate<void(TArrayRef), rome::tgt_opt>;
template class ::rome::delegate<TArrayRef(TArrayRef), rome::tgt_dyn_req>;
template class ::rome::delegate<TArrayRef(TArrayRef), rome::tgt_stat_req>;
static_assert(test_rome_delegate::is_target_req_error_produced<
                  ::rome::delegate<TArrayRef(TArrayRef), rome::tgt_opt>>,
    "");

using TArrayPtr = int (*)[10];
template class ::rome::delegate<void(TArrayPtr), rome::tgt_dyn_req>;
template class ::rome::delegate<void(TArrayPtr), rome::tgt_stat_req>;
template class ::rome::delegate<void(TArrayPtr), rome::tgt_opt>;
template class ::rome::delegate<TArrayPtr(TArrayPtr), rome::tgt_dyn_req>;
template class ::rome::delegate<TArrayPtr(TArrayPtr), rome::tgt_stat_req>;
static_assert(test_rome_delegate::is_target_req_error_produced<
                  ::rome::delegate<TArrayPtr(TArrayPtr), rome::tgt_opt>>,
    "");

using TFunctionRef = int (&)(int);
template class ::rome::delegate<void(TFunctionRef), rome::tgt_dyn_req>;
template class ::rome::delegate<void(TFunctionRef), rome::tgt_stat_req>;
template class ::rome::delegate<void(TFunctionRef), rome::tgt_opt>;
template class ::rome::delegate<TFunctionRef(TFunctionRef), rome::tgt_dyn_req>;
template class ::rome::delegate<TFunctionRef(TFunctionRef), rome::tgt_stat_req>;
static_assert(test_rome_delegate::is_target_req_error_produced<
                  ::rome::delegate<TFunctionRef(TFunctionRef), rome::tgt_opt>>,
    "");

using TFunctionPtr = int (*)(int);
template class ::rome::delegate<void(TFunctionPtr), rome::tgt_dyn_req>;
template class ::rome::delegate<void(TFunctionPtr), rome::tgt_stat_req>;
template class ::rome::delegate<void(TFunctionPtr), rome::tgt_opt>;
template class ::rome::delegate<TFunctionPtr(TFunctionPtr), rome::tgt_dyn_req>;
template class ::rome::delegate<TFunctionPtr(TFunctionPtr), rome::tgt_stat_req>;
static_assert(test_rome_delegate::is_target_req_error_produced<
                  ::rome::delegate<TFunctionPtr(TFunctionPtr), rome::tgt_opt>>,
    "");

TEST_CASE("rome::delegate - explicit template instantiations") {
}
