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

TEST_SUITE_BEGIN("header file: rome/delegate.hpp");

namespace {

template<typename TDelegate>
constexpr bool produces_expected_behavior_error =
    std::is_base_of<rome::detail::invalid_delegate_expected_behavior, TDelegate>::value;

template<typename Signature>
struct tester;

template<typename Ret, typename... Args>
struct tester<Ret(Args...)> {
    static constexpr auto expectedSize = sizeof(void*) + 2 * sizeof(void (*)());
    // TODO: explicitly test all functions that explicitly arent provided! (Synopsis)
    static constexpr bool testMandatory() {
        using delegate_type = rome::delegate<Ret(Args...), rome::target_is_mandatory>;
        static_assert(std::is_nothrow_move_constructible<delegate_type>::value, "");
        static_assert(std::is_nothrow_move_assignable<delegate_type>::value, "");
        static_assert(std::is_nothrow_destructible<delegate_type>::value, "");
        static_assert(!std::is_default_constructible<delegate_type>::value, "");
        static_assert(!std::is_copy_constructible<delegate_type>::value, "");
        static_assert(!std::is_copy_assignable<delegate_type>::value, "");
        static_assert(!produces_expected_behavior_error<delegate_type>, "");
        static_assert(sizeof(delegate_type) <= expectedSize, "");
        return true;
    }
    static constexpr bool testExpected() {
        using delegate_type = rome::delegate<Ret(Args...), rome::target_is_expected>;
        static_assert(std::is_nothrow_default_constructible<delegate_type>::value, "");
        static_assert(std::is_nothrow_move_constructible<delegate_type>::value, "");
        static_assert(std::is_nothrow_move_assignable<delegate_type>::value, "");
        static_assert(std::is_nothrow_destructible<delegate_type>::value, "");
        static_assert(!std::is_copy_constructible<delegate_type>::value, "");
        static_assert(!std::is_copy_assignable<delegate_type>::value, "");
        static_assert(!produces_expected_behavior_error<delegate_type>, "");
        static_assert(sizeof(delegate_type) <= expectedSize, "");
        return true;
    }
    template<typename T = Ret, std::enable_if_t<std::is_same<T, void>::value, int> = 0>
    static constexpr bool testOptional() {
        using delegate_type = rome::delegate<Ret(Args...), rome::target_is_optional>;
        static_assert(std::is_nothrow_default_constructible<delegate_type>::value, "");
        static_assert(std::is_nothrow_move_constructible<delegate_type>::value, "");
        static_assert(std::is_nothrow_move_assignable<delegate_type>::value, "");
        static_assert(std::is_nothrow_destructible<delegate_type>::value, "");
        static_assert(!std::is_copy_constructible<delegate_type>::value, "");
        static_assert(!std::is_copy_assignable<delegate_type>::value, "");
        static_assert(!produces_expected_behavior_error<delegate_type>, "");
        static_assert(sizeof(delegate_type) <= expectedSize, "");
        return true;
    }
    template<typename T = Ret, std::enable_if_t<!std::is_same<T, void>::value, int> = 0>
    static constexpr bool testOptional() {
        using delegate_type = rome::delegate<Ret(Args...), rome::target_is_optional>;
        static_assert(produces_expected_behavior_error<delegate_type>, "");
        return true;
    }
};
}  // namespace

// explicit template instantiations

#define TEST_WITH_VOID_RETURN_AND_ARGS(Args)                                                       \
    template class ::rome::delegate<void Args, rome::target_is_mandatory>;                         \
    static_assert(tester<void Args>::testMandatory(), "");                                         \
    template class ::rome::delegate<void Args, rome::target_is_expected>;                          \
    static_assert(tester<void Args>::testExpected(), "");                                          \
    template class ::rome::delegate<void Args, rome::target_is_optional>;                          \
    static_assert(tester<void Args>::testOptional(), "")

#define TEST_WITH_RETURN_AND_ARGS(Ret, Args)                                                       \
    static_assert(                                                                                 \
        !std::is_void<Ret>::value, "Only non void arguments 'Ret' for this macro are allowed!");   \
    template class ::rome::delegate<Ret Args, rome::target_is_mandatory>;                          \
    static_assert(tester<Ret Args>::testMandatory(), "");                                          \
    template class ::rome::delegate<Ret Args, rome::target_is_expected>;                           \
    static_assert(tester<Ret Args>::testExpected(), "");                                           \
    /* explicit template instantiation with void return and rome::target_is_optional would lead    \
    to a compile error */                                                                          \
    static_assert(tester<Ret Args>::testOptional(), "")

TEST_WITH_VOID_RETURN_AND_ARGS(());
TEST_WITH_RETURN_AND_ARGS(int, ());
TEST_WITH_VOID_RETURN_AND_ARGS((int));
TEST_WITH_RETURN_AND_ARGS(int, (int));

enum E : int { e_zero, e_one };
TEST_WITH_VOID_RETURN_AND_ARGS((E));
TEST_WITH_RETURN_AND_ARGS(E, (E));

enum class EC : int { zero, one };
TEST_WITH_VOID_RETURN_AND_ARGS((EC));
TEST_WITH_RETURN_AND_ARGS(EC, (EC));

union U {
    int i;
};
TEST_WITH_VOID_RETURN_AND_ARGS((U));
TEST_WITH_RETURN_AND_ARGS(U, (U));

struct C {
    int i;
    int memberFunction(int) {
        return i;
    }
    int constMemberFunction(int) const {
        return i;
    }
};
TEST_WITH_VOID_RETURN_AND_ARGS((C));
TEST_WITH_RETURN_AND_ARGS(C, (C));
TEST_WITH_VOID_RETURN_AND_ARGS((C&));
TEST_WITH_RETURN_AND_ARGS(C&, (C&));
TEST_WITH_VOID_RETURN_AND_ARGS((const C&));
TEST_WITH_RETURN_AND_ARGS(const C&, (const C&));
TEST_WITH_VOID_RETURN_AND_ARGS((C &&));
TEST_WITH_RETURN_AND_ARGS(C&&, (C &&));
TEST_WITH_VOID_RETURN_AND_ARGS((C*));
TEST_WITH_RETURN_AND_ARGS(C*, (C*));
TEST_WITH_VOID_RETURN_AND_ARGS((const C*));
TEST_WITH_RETURN_AND_ARGS(const C*, (const C*));

using TMemberObject = int C::*;
TEST_WITH_VOID_RETURN_AND_ARGS((TMemberObject));
TEST_WITH_RETURN_AND_ARGS(TMemberObject, (TMemberObject));

using TMemberFunction = int (C::*)(int);
TEST_WITH_VOID_RETURN_AND_ARGS((TMemberFunction));
TEST_WITH_RETURN_AND_ARGS(TMemberFunction, (TMemberFunction));

using TConstMemberFunction = int (C::*)(int) const;
TEST_WITH_VOID_RETURN_AND_ARGS((TConstMemberFunction));
TEST_WITH_RETURN_AND_ARGS(TConstMemberFunction, (TConstMemberFunction));

using TArray = int[10];
TEST_WITH_VOID_RETURN_AND_ARGS((TArray));

using TArrayRef = int (&)[10];
TEST_WITH_VOID_RETURN_AND_ARGS((TArrayRef));
TEST_WITH_RETURN_AND_ARGS(TArrayRef, (TArrayRef));

using TArrayPtr = int (*)[10];
TEST_WITH_VOID_RETURN_AND_ARGS((TArrayPtr));
TEST_WITH_RETURN_AND_ARGS(TArrayPtr, (TArrayPtr));

using TFunctionRef = int (&)(int);
TEST_WITH_VOID_RETURN_AND_ARGS((TFunctionRef));
TEST_WITH_RETURN_AND_ARGS(TFunctionRef, (TFunctionRef));

using TFunctionPtr = int (*)(int);
TEST_WITH_VOID_RETURN_AND_ARGS((TFunctionPtr));
TEST_WITH_RETURN_AND_ARGS(TFunctionPtr, (TFunctionPtr));

namespace {
// helpers to create a whole lot of function argument combinations of values, pointers, references
// and cv-modifiers
template<typename... T>
using add_ref = int(T..., T&..., T&&...);

template<typename... T>
using add_cv = add_ref<T..., T const..., T volatile..., T const volatile...>;

template<typename... T>
using add_ptr = add_cv<T..., T*..., T const*..., T volatile*..., T const volatile*...>;

template<typename... T>
using add_ptr_ptr = add_ptr<T..., T*..., T const*..., T volatile*..., T const volatile*...>;

template<typename... T>
using add_ptr_ptr_ptr = add_ptr_ptr<T..., T*..., T const*..., T volatile*..., T const volatile*...>;
}  // namespace
using TSignature = add_ptr_ptr_ptr<C>;
TEST_WITH_VOID_RETURN_AND_ARGS((TSignature));
TEST_WITH_RETURN_AND_ARGS(int, (TSignature));

TEST_CASE("rome::delegate - template parameter combinations") {
    CHECK_MESSAGE(
        true, "explicit template instantiations build with different possible type combinations");
    CHECK_MESSAGE(true, "templates of different types fail to instantiate if ExpectedBehavior is "
                        "rome::target_is_optional and the return type is not void");
    CHECK_MESSAGE(true, "check size of class");
    CHECK_MESSAGE(true, "check default constructor");
    CHECK_MESSAGE(true, "check copy constructor");
    CHECK_MESSAGE(true, "check copy assignment");
    CHECK_MESSAGE(true, "check move constructor");
    CHECK_MESSAGE(true, "check move assignment");
    CHECK_MESSAGE(true, "check destructor");
    CHECK_MESSAGE(false, "TODO test other functions which should be explicitly deleted or be available!");
}

TEST_SUITE_END();  // rome/delegate.hpp
