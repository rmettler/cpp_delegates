//
// Project: C++ delegates
//
// Copyright Roger Mettler 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//


#include <rome/delegate.hpp>

#include <doctest/doctest.h>
#include <test/delegate_traits.hpp>
#include <test/doctest_extensions.hpp>
#include <type_traits>


// NOLINTNEXTLINE(misc-use-anonymous-namespace)
TEST_CASE("By default, the empty behavior is target_is_expected.") {
    STATIC_REQUIRE(
        std::is_same<rome::delegate<void()>, rome::delegate<void(), rome::target_is_expected>>{});
    STATIC_REQUIRE(
        std::is_same<rome::delegate<bool()>, rome::delegate<bool(), rome::target_is_expected>>{});
    STATIC_REQUIRE(std::is_same<rome::fwd_delegate<void()>,
        rome::fwd_delegate<void(), rome::target_is_expected>>{});
}


// Returns true if swapping T with both global and member swap is noexcept.
// Compile error if T is not swappable with swap(T&, T&) and (T&).swap(T&).
template<typename T>
struct is_nothrow_swappable {
    constexpr operator bool() {
        using std::swap;
        constexpr auto global = noexcept(swap(std::declval<T&>(), std::declval<T&>()));
        constexpr auto member = noexcept(std::declval<T&>().swap(std::declval<T&>()));
        return global && member;
    }
};


// Returns true if explicitly converting T to U is noexcept.
// Compile error if T is not convertable to U at all.
template<typename T, typename U>
struct is_nothrow_explicit_convertible_to {
    constexpr operator bool() {
        return noexcept(static_cast<U>(std::declval<T>()));
    }
};


// Returns true if calling T with the arguments Args... is noexcept.
// Compile error if T is not callable with Args... at all.
template<typename T, typename... Args>
struct is_throwing_callable_with {
    constexpr operator bool() {
        return !noexcept(std::declval<T&>()(std::declval<Args>()...));
    }
};


// Checks the basic type trait requirements of all delegates
template<typename Delegate, typename = test::delegate_signature_t<Delegate>>
struct base_requirements_checker;

template<typename Delegate, typename Ret, typename... Args>
struct base_requirements_checker<Delegate, Ret(Args...)> {
    // NOLINTNEXTLINE(readability-function-cognitive-complexity)
    void operator()() const {
        STATIC_REQUIRE(sizeof(Delegate) <= sizeof(void*) + 2 * sizeof(void (*)()));
        STATIC_REQUIRE(std::is_nothrow_destructible<Delegate>{});
        STATIC_REQUIRE(std::is_nothrow_move_constructible<Delegate>{});
        STATIC_REQUIRE(std::is_nothrow_move_assignable<Delegate>{});
        STATIC_REQUIRE(!std::is_copy_constructible<Delegate>{});
        STATIC_REQUIRE(!std::is_copy_assignable<Delegate>{});
        STATIC_REQUIRE(is_nothrow_swappable<Delegate>{});
        STATIC_REQUIRE(is_nothrow_explicit_convertible_to<const Delegate, bool>{});
        STATIC_REQUIRE(!std::is_convertible<Delegate, bool>{});
        STATIC_REQUIRE(is_throwing_callable_with<const Delegate, Args...>{});
    }
};

template<typename Delegate>
constexpr base_requirements_checker<Delegate> check_base_requirements{};


// Checks the type trait requirements of delegates their empty behavior set to target_is_mandatory
template<typename Delegate,
    std::enable_if_t<std::is_same<test::delegate_behavior_t<Delegate>, rome::target_is_mandatory>{},
        int> = 0>
void check_type_requirements() {
    check_base_requirements<Delegate>();
    STATIC_REQUIRE(!std::is_default_constructible<Delegate>{});
    STATIC_REQUIRE(!std::is_constructible<Delegate, decltype(nullptr)>{});
    STATIC_REQUIRE(!std::is_assignable<Delegate, decltype(nullptr)>{});
}


// Checks the type trait requirements of delegates their empty behavior set to target_is_expected or
// targe_is_optional
template<typename Delegate,
    std::enable_if_t<
        std::is_same<test::delegate_behavior_t<Delegate>, rome::target_is_expected>{}
            || std::is_same<test::delegate_behavior_t<Delegate>, rome::target_is_optional>{},
        int> = 0>
void check_type_requirements() {
    check_base_requirements<Delegate>();
    STATIC_REQUIRE(std::is_nothrow_default_constructible<Delegate>{});
    STATIC_REQUIRE(std::is_nothrow_constructible<Delegate, decltype(nullptr)>{});
    STATIC_REQUIRE(std::is_nothrow_assignable<Delegate, decltype(nullptr)>{});
}


// helpers to create a whole lot of function argument combinations of values, pointers, references
// and cv-modifiers
template<typename Ret, typename... T>
using add_ref = Ret(T..., T&..., T&&...);

template<typename Ret, typename... T>
using add_cv = add_ref<Ret, T..., T const..., T volatile..., T const volatile...>;

template<typename Ret, typename... T>
using add_ptr = add_cv<Ret, T..., T*..., T const*..., T volatile*..., T const volatile*...>;

template<typename Ret, typename... T>
using add_ptr_ptr = add_ptr<Ret, T..., T*..., T const*..., T volatile*..., T const volatile*...>;

template<typename Ret, typename... T>
using make_long_signature =
    add_ptr_ptr<Ret, T..., T*..., T const*..., T volatile*..., T const volatile*...>;

// Types for testing
enum E : int { e_zero, e_one };
enum class EC : int { zero, one };
union U {
    int i;
};
struct C {
    int i;  // NOLINT(misc-non-private-member-variables-in-classes)
    auto memberFunction(int) -> int {
        return ++i;
    }
    auto constMemberFunction(int) const -> int {
        return i;
    }
};
// NOLINTBEGIN(*-avoid-c-arrays,*-magic-numbers)
using MemberObject           = int C::*;
using ConstMemberObject      = const int C::*;
using MemberFunctionPtr      = int (C::*)(int);
using ConstMemberFunctionPtr = int (C::*)(int) const;
using Array                  = int[10];
using ArrayRef               = int (&)[10];
using ConstArrayRef          = const int (&)[10];
using ArrayPtr               = int (*)[10];
using ConstArrayPtr          = const int (*)[10];
using FunctionRef            = int (&)(int);
using FunctionPtr            = int (*)(int);
// NOLINTEND(*-avoid-c-arrays,*-magic-numbers)

// clang-format off
using void_return_and_immutable_args = std::tuple<
    void(),
    void(int),
    void(E),
    void(EC),
    void(U),
    void(C),
    void(const C&),
    void(C&&),
    void(const C*),
    void(ConstMemberObject),
    void(ConstMemberFunctionPtr),
    void(ConstArrayRef),
    void(ConstArrayPtr),
    void(FunctionRef),
    void(FunctionPtr)
>;

using void_return_and_mutable_args = std::tuple<
    make_long_signature<void, C>,
    void(C&),
    void(C*),
    void(MemberObject),
    void(MemberFunctionPtr),
    void(Array),
    void(ArrayRef),
    void(ArrayPtr)
>;

using non_void_return_and_immutable_args = std::tuple<
    int(),
    int(int),
    E(E),
    EC(EC),
    U(U),
    C(C),
    const C&(const C&),
    C&&(C&&),
    const C*(const C*),
    ConstMemberObject(ConstMemberObject),
    ConstMemberFunctionPtr(ConstMemberFunctionPtr),
    ConstArrayRef(ConstArrayRef),
    ConstArrayPtr(ConstArrayPtr),
    FunctionRef(FunctionRef),
    FunctionPtr(FunctionPtr)
>;

using non_void_return_and_mutable_args = std::tuple<
    make_long_signature<int, C>,
    C&(C&),
    C*(C*),
    MemberObject(MemberObject),
    MemberFunctionPtr(MemberFunctionPtr),
    ArrayRef(ArrayRef),
    ArrayPtr(ArrayPtr)
>;
// clang-format on


// NOLINTNEXTLINE(misc-use-anonymous-namespace)
TEST_CASE_TEMPLATE_DEFINE(
    "The rome::delegate with non-optional target satisfy the type requirements. ", Signature,
    delegate_non_optional_target) {
    check_type_requirements<rome::delegate<Signature, rome::target_is_expected>>();
    check_type_requirements<rome::delegate<Signature, rome::target_is_mandatory>>();
}
TEST_CASE_TEMPLATE_APPLY(delegate_non_optional_target, void_return_and_immutable_args);
TEST_CASE_TEMPLATE_APPLY(delegate_non_optional_target, void_return_and_mutable_args);
TEST_CASE_TEMPLATE_APPLY(delegate_non_optional_target, non_void_return_and_immutable_args);
TEST_CASE_TEMPLATE_APPLY(delegate_non_optional_target, non_void_return_and_mutable_args);


// NOLINTNEXTLINE(misc-use-anonymous-namespace)
TEST_CASE_TEMPLATE_DEFINE(
    "The rome::delegate with void return and optional target satisfy the type requirements. ",
    Signature, delegate_optional_target) {
    check_type_requirements<rome::delegate<Signature, rome::target_is_optional>>();
}
TEST_CASE_TEMPLATE_APPLY(delegate_optional_target, void_return_and_immutable_args);
TEST_CASE_TEMPLATE_APPLY(delegate_optional_target, void_return_and_mutable_args);


// NOLINTNEXTLINE(misc-use-anonymous-namespace)
TEST_CASE_TEMPLATE_DEFINE(
    "The rome::fwd_delegate satisfy the type requirements. ", Signature, fwd_delegate) {
    check_type_requirements<rome::fwd_delegate<Signature, rome::target_is_optional>>();
    check_type_requirements<rome::fwd_delegate<Signature, rome::target_is_expected>>();
    check_type_requirements<rome::fwd_delegate<Signature, rome::target_is_mandatory>>();
}
TEST_CASE_TEMPLATE_APPLY(fwd_delegate, void_return_and_immutable_args);
