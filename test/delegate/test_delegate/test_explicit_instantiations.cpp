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

template<typename TDelegate>
constexpr bool produces_character_error =
    std::is_base_of<rome::detail::invalid_delegate_character, TDelegate>::value;

// explicit template instantiations
template class ::rome::delegate<void(), rome::target_expected>;
template class ::rome::delegate<void(), rome::target_enforced>;
template class ::rome::delegate<void(), rome::target_optional>;
template class ::rome::delegate<void(int), rome::target_expected>;
template class ::rome::delegate<void(int), rome::target_enforced>;
template class ::rome::delegate<void(int), rome::target_optional>;
template class ::rome::delegate<int(), rome::target_expected>;
template class ::rome::delegate<int(), rome::target_enforced>;
static_assert(produces_character_error<::rome::delegate<int(), rome::target_optional>>, "");
template class ::rome::delegate<int(int), rome::target_expected>;
template class ::rome::delegate<int(int), rome::target_enforced>;
static_assert(produces_character_error<::rome::delegate<int(int), rome::target_optional>>, "");

enum E : int { e_zero, e_one };
template class ::rome::delegate<void(E), rome::target_expected>;
template class ::rome::delegate<void(E), rome::target_enforced>;
template class ::rome::delegate<void(E), rome::target_optional>;
template class ::rome::delegate<E(E), rome::target_expected>;
template class ::rome::delegate<E(E), rome::target_enforced>;
static_assert(produces_character_error<::rome::delegate<E(E), rome::target_optional>>, "");

enum class EC : int { zero, one };
template class ::rome::delegate<void(EC), rome::target_expected>;
template class ::rome::delegate<void(EC), rome::target_enforced>;
template class ::rome::delegate<void(EC), rome::target_optional>;
template class ::rome::delegate<EC(EC), rome::target_expected>;
template class ::rome::delegate<EC(EC), rome::target_enforced>;
static_assert(produces_character_error<::rome::delegate<EC(EC), rome::target_optional>>, "");

union U {
    int i;
};
template class ::rome::delegate<void(U), rome::target_expected>;
template class ::rome::delegate<void(U), rome::target_enforced>;
template class ::rome::delegate<void(U), rome::target_optional>;
template class ::rome::delegate<U(U), rome::target_expected>;
template class ::rome::delegate<U(U), rome::target_enforced>;
static_assert(produces_character_error<::rome::delegate<U(U), rome::target_optional>>, "");

struct C {
    int i;
    int memberFunction(int) {
        return i;
    }
    int constMemberFunction(int) const {
        return i;
    }
};
template class ::rome::delegate<void(C), rome::target_expected>;
template class ::rome::delegate<void(C), rome::target_enforced>;
template class ::rome::delegate<void(C), rome::target_optional>;
template class ::rome::delegate<C(C), rome::target_expected>;
template class ::rome::delegate<C(C), rome::target_enforced>;
static_assert(produces_character_error<::rome::delegate<C(C), rome::target_optional>>, "");
template class ::rome::delegate<void(C&), rome::target_expected>;
template class ::rome::delegate<void(C&), rome::target_enforced>;
template class ::rome::delegate<void(C&), rome::target_optional>;
template class ::rome::delegate<C&(C&), rome::target_expected>;
template class ::rome::delegate<C&(C&), rome::target_enforced>;
static_assert(produces_character_error<::rome::delegate<C&(C&), rome::target_optional>>, "");
template class ::rome::delegate<void(const C&), rome::target_expected>;
template class ::rome::delegate<void(const C&), rome::target_enforced>;
template class ::rome::delegate<void(const C&), rome::target_optional>;
template class ::rome::delegate<const C&(const C&), rome::target_expected>;
template class ::rome::delegate<const C&(const C&), rome::target_enforced>;
static_assert(
    produces_character_error<::rome::delegate<const C&(const C&), rome::target_optional>>, "");
template class ::rome::delegate<void(C&&), rome::target_expected>;
template class ::rome::delegate<void(C&&), rome::target_enforced>;
template class ::rome::delegate<void(C&&), rome::target_optional>;
template class ::rome::delegate<C && (C &&), rome::target_expected>;
template class ::rome::delegate<C && (C &&), rome::target_enforced>;
static_assert(produces_character_error<::rome::delegate<C && (C &&), rome::target_optional>>, "");
template class ::rome::delegate<void(C*), rome::target_expected>;
template class ::rome::delegate<void(C*), rome::target_enforced>;
template class ::rome::delegate<void(C*), rome::target_optional>;
template class ::rome::delegate<C*(C*), rome::target_expected>;
template class ::rome::delegate<C*(C*), rome::target_enforced>;
static_assert(produces_character_error<::rome::delegate<C*(C*), rome::target_optional>>, "");
template class ::rome::delegate<void(const C*), rome::target_expected>;
template class ::rome::delegate<void(const C*), rome::target_enforced>;
template class ::rome::delegate<void(const C*), rome::target_optional>;
template class ::rome::delegate<const C*(const C*), rome::target_expected>;
template class ::rome::delegate<const C*(const C*), rome::target_enforced>;
static_assert(
    produces_character_error<::rome::delegate<const C*(const C*), rome::target_optional>>, "");

using TMemberObject = int C::*;
template class ::rome::delegate<void(TMemberObject), rome::target_expected>;
template class ::rome::delegate<void(TMemberObject), rome::target_enforced>;
template class ::rome::delegate<void(TMemberObject), rome::target_optional>;
template class ::rome::delegate<TMemberObject(TMemberObject), rome::target_expected>;
template class ::rome::delegate<TMemberObject(TMemberObject), rome::target_enforced>;
static_assert(
    produces_character_error<::rome::delegate<TMemberObject(TMemberObject), rome::target_optional>>,
    "");

using TMemberFunction = int (C::*)(int);
template class ::rome::delegate<void(TMemberFunction), rome::target_expected>;
template class ::rome::delegate<void(TMemberFunction), rome::target_enforced>;
template class ::rome::delegate<void(TMemberFunction), rome::target_optional>;
template class ::rome::delegate<TMemberFunction(TMemberFunction), rome::target_expected>;
template class ::rome::delegate<TMemberFunction(TMemberFunction), rome::target_enforced>;
static_assert(produces_character_error<
                  ::rome::delegate<TMemberFunction(TMemberFunction), rome::target_optional>>,
    "");

using TConstMemberFunction = int (C::*)(int) const;
template class ::rome::delegate<void(TConstMemberFunction), rome::target_expected>;
template class ::rome::delegate<void(TConstMemberFunction), rome::target_enforced>;
template class ::rome::delegate<void(TConstMemberFunction), rome::target_optional>;
template class ::rome::delegate<TConstMemberFunction(TConstMemberFunction), rome::target_expected>;
template class ::rome::delegate<TConstMemberFunction(TConstMemberFunction), rome::target_enforced>;
static_assert(
    produces_character_error<
        ::rome::delegate<TConstMemberFunction(TConstMemberFunction), rome::target_optional>>,
    "");

using TArray = int[10];
template class ::rome::delegate<void(TArray), rome::target_expected>;
template class ::rome::delegate<void(TArray), rome::target_enforced>;
template class ::rome::delegate<void(TArray), rome::target_optional>;

using TArrayRef = int (&)[10];
template class ::rome::delegate<void(TArrayRef), rome::target_expected>;
template class ::rome::delegate<void(TArrayRef), rome::target_enforced>;
template class ::rome::delegate<void(TArrayRef), rome::target_optional>;
template class ::rome::delegate<TArrayRef(TArrayRef), rome::target_expected>;
template class ::rome::delegate<TArrayRef(TArrayRef), rome::target_enforced>;
static_assert(
    produces_character_error<::rome::delegate<TArrayRef(TArrayRef), rome::target_optional>>, "");

using TArrayPtr = int (*)[10];
template class ::rome::delegate<void(TArrayPtr), rome::target_expected>;
template class ::rome::delegate<void(TArrayPtr), rome::target_enforced>;
template class ::rome::delegate<void(TArrayPtr), rome::target_optional>;
template class ::rome::delegate<TArrayPtr(TArrayPtr), rome::target_expected>;
template class ::rome::delegate<TArrayPtr(TArrayPtr), rome::target_enforced>;
static_assert(
    produces_character_error<::rome::delegate<TArrayPtr(TArrayPtr), rome::target_optional>>, "");

using TFunctionRef = int (&)(int);
template class ::rome::delegate<void(TFunctionRef), rome::target_expected>;
template class ::rome::delegate<void(TFunctionRef), rome::target_enforced>;
template class ::rome::delegate<void(TFunctionRef), rome::target_optional>;
template class ::rome::delegate<TFunctionRef(TFunctionRef), rome::target_expected>;
template class ::rome::delegate<TFunctionRef(TFunctionRef), rome::target_enforced>;
static_assert(
    produces_character_error<::rome::delegate<TFunctionRef(TFunctionRef), rome::target_optional>>,
    "");

using TFunctionPtr = int (*)(int);
template class ::rome::delegate<void(TFunctionPtr), rome::target_expected>;
template class ::rome::delegate<void(TFunctionPtr), rome::target_enforced>;
template class ::rome::delegate<void(TFunctionPtr), rome::target_optional>;
template class ::rome::delegate<TFunctionPtr(TFunctionPtr), rome::target_expected>;
template class ::rome::delegate<TFunctionPtr(TFunctionPtr), rome::target_enforced>;
static_assert(
    produces_character_error<::rome::delegate<TFunctionPtr(TFunctionPtr), rome::target_optional>>,
    "");

TEST_CASE("rome::delegate - explicit template instantiations") {
}
