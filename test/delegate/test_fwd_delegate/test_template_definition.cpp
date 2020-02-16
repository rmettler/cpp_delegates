//
// Project: C++ delegates
//
// Copyright Roger Mettler 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <doctest.h>

#include <rome/delegate.hpp>
#include <type_traits>

TEST_SUITE_BEGIN("header file: rome/fwd_delegate.hpp");

namespace {

DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wunused-template")

template<typename T, decltype(T{nullptr}, int{}) = 0,
    std::enable_if_t<noexcept(std::declval<T>().operator=(nullptr)), int> = 0>
constexpr std::true_type is_nothrow_nullptr_constructible_impl(int);

template<typename T>
constexpr std::false_type is_nothrow_nullptr_constructible_impl(...);

// check whether T can be constructed with nullptr
template<typename T>
struct is_nothrow_nullptr_constructible
    : decltype(is_nothrow_nullptr_constructible_impl<T>(int{})) {};

template<typename T, T& (T::*)(std::nullptr_t) = (&T::operator=),
    std::enable_if_t<noexcept(std::declval<T>().operator=(nullptr)), int> = 0>
constexpr std::true_type is_nothrow_nullptr_assignable_impl(int);

template<typename T>
constexpr std::false_type is_nothrow_nullptr_assignable_impl(...);

// check whether nullptr can be assigned to T
template<typename T>
struct is_nothrow_nullptr_assignable : decltype(is_nothrow_nullptr_assignable_impl<T>(int{})) {};

template<typename T, decltype(static_cast<bool>(std::declval<T>())) = true,
    std::enable_if_t<noexcept(static_cast<bool>(std::declval<T>())), int> = 0>
constexpr std::true_type is_nothrow_bool_castable_impl(int);

template<typename T>
constexpr std::false_type is_nothrow_bool_castable_impl(...);

// check whether T can be casted to bool or not
template<typename T>
struct is_nothrow_bool_castable : decltype(is_nothrow_bool_castable_impl<T>(int{})) {};

template<typename T, void (T::*)(T&) = (&T::swap),
    std::enable_if_t<noexcept(std::declval<T>().swap(std::declval<T&>())), int> = 0>
constexpr std::true_type is_nothrow_swappable_impl(int);

template<typename T>
constexpr std::false_type is_nothrow_swappable_impl(...);

// check whether T can be swapped with T
template<typename T>
struct is_nothrow_swappable : decltype(is_nothrow_swappable_impl<T>(int{})) {};

template<typename T, typename Ret, typename... Args, Ret (T::*)(Args...) const = &T::operator()>
constexpr std::true_type is_const_callable_with_impl(int);

template<typename T, typename Ret, typename... Args>
constexpr std::false_type is_const_callable_with_impl(...);

// check whether T can be called with argument types Args... and return type Ret
template<typename T, typename Ret, typename... Args>
struct is_const_callable_with : decltype(is_const_callable_with_impl<T, Ret, Args...>(int{})) {};

DOCTEST_CLANG_SUPPRESS_WARNING_POP

// check whether a compile error would occur because Behavior argument is not correct
template<typename TDelegate>
constexpr bool produces_template_param_error =
    std::is_base_of<rome::detail::bad_delegate_template_param, TDelegate>::value;

template<typename TDelegate>
constexpr bool produces_mutable_argument_error =
    std::is_base_of<rome::detail::bad_delegate_template_param_with_mutable_arguments,
        TDelegate>::value;

template<typename Signature>
struct tester;

template<typename... Args>
struct tester<void(Args...)> {
    static constexpr auto expectedSize = sizeof(void*) + 2 * sizeof(void (*)());
    static constexpr bool testMandatory() {
        using delegate_type = rome::fwd_delegate<void(Args...), rome::target_is_mandatory>;
        static_assert(!std::is_default_constructible<delegate_type>::value, "");
        static_assert(!std::is_copy_constructible<delegate_type>::value, "");
        static_assert(!std::is_copy_assignable<delegate_type>::value, "");
        static_assert(std::is_nothrow_move_constructible<delegate_type>::value, "");
        static_assert(std::is_nothrow_move_assignable<delegate_type>::value, "");
        static_assert(std::is_nothrow_destructible<delegate_type>::value, "");
        static_assert(!is_nothrow_nullptr_constructible<delegate_type>::value, "");
        static_assert(!is_nothrow_nullptr_assignable<delegate_type>::value, "");
        static_assert(is_nothrow_bool_castable<delegate_type>::value, "");
        static_assert(is_nothrow_swappable<delegate_type>::value, "");
        static_assert(is_const_callable_with<delegate_type, void, Args...>::value, "");
        static_assert(!produces_template_param_error<delegate_type>, "");
        static_assert(sizeof(delegate_type) <= expectedSize, "");
        return true;
    }
    static constexpr bool testExpected() {
        using delegate_type = rome::fwd_delegate<void(Args...), rome::target_is_expected>;
        static_assert(std::is_nothrow_default_constructible<delegate_type>::value, "");
        static_assert(!std::is_copy_constructible<delegate_type>::value, "");
        static_assert(!std::is_copy_assignable<delegate_type>::value, "");
        static_assert(std::is_nothrow_move_constructible<delegate_type>::value, "");
        static_assert(std::is_nothrow_move_assignable<delegate_type>::value, "");
        static_assert(std::is_nothrow_destructible<delegate_type>::value, "");
        static_assert(is_nothrow_nullptr_constructible<delegate_type>::value, "");
        static_assert(is_nothrow_nullptr_assignable<delegate_type>::value, "");
        static_assert(is_nothrow_bool_castable<delegate_type>::value, "");
        static_assert(is_nothrow_swappable<delegate_type>::value, "");
        static_assert(is_const_callable_with<delegate_type, void, Args...>::value, "");
        static_assert(!produces_template_param_error<delegate_type>, "");
        static_assert(sizeof(delegate_type) <= expectedSize, "");
        return true;
    }
    static constexpr bool testOptional() {
        using delegate_type = rome::fwd_delegate<void(Args...), rome::target_is_optional>;
        static_assert(std::is_nothrow_default_constructible<delegate_type>::value, "");
        static_assert(!std::is_copy_constructible<delegate_type>::value, "");
        static_assert(!std::is_copy_assignable<delegate_type>::value, "");
        static_assert(std::is_nothrow_move_constructible<delegate_type>::value, "");
        static_assert(std::is_nothrow_move_assignable<delegate_type>::value, "");
        static_assert(std::is_nothrow_destructible<delegate_type>::value, "");
        static_assert(is_nothrow_nullptr_constructible<delegate_type>::value, "");
        static_assert(is_nothrow_nullptr_assignable<delegate_type>::value, "");
        static_assert(is_nothrow_bool_castable<delegate_type>::value, "");
        static_assert(is_nothrow_swappable<delegate_type>::value, "");
        static_assert(is_const_callable_with<delegate_type, void, Args...>::value, "");
        static_assert(!produces_template_param_error<delegate_type>, "");
        static_assert(sizeof(delegate_type) <= expectedSize, "");
        return true;
    }
};

}  // namespace

// explicit template instantiations

#define TEST_WITH_VOID_RETURN_AND_ARGS(Args)                                                       \
    template class rome::fwd_delegate<void Args, rome::target_is_mandatory>;                       \
    static_assert(tester<void Args>::testMandatory(), "");                                         \
    static_assert(                                                                                 \
        !produces_template_param_error<rome::fwd_delegate<void Args, rome::target_is_mandatory>>,  \
        "");                                                                                       \
    static_assert(!produces_mutable_argument_error<                                                \
                      rome::fwd_delegate<void Args, rome::target_is_mandatory>>,                   \
        "");                                                                                       \
    template class rome::fwd_delegate<void Args, rome::target_is_expected>;                        \
    static_assert(tester<void Args>::testExpected(), "");                                          \
    static_assert(                                                                                 \
        !produces_template_param_error<rome::fwd_delegate<void Args, rome::target_is_expected>>,   \
        "");                                                                                       \
    static_assert(                                                                                 \
        !produces_mutable_argument_error<rome::fwd_delegate<void Args, rome::target_is_expected>>, \
        "");                                                                                       \
    template class rome::fwd_delegate<void Args, rome::target_is_optional>;                        \
    static_assert(tester<void Args>::testOptional(), "");                                          \
    static_assert(                                                                                 \
        !produces_template_param_error<rome::fwd_delegate<void Args, rome::target_is_optional>>,   \
        "");                                                                                       \
    static_assert(                                                                                 \
        !produces_mutable_argument_error<rome::fwd_delegate<void Args, rome::target_is_optional>>, \
        "");                                                                                       \
    static_assert(produces_template_param_error<rome::fwd_delegate<void Args, int>>, "")

#define TEST_PRODUCES_MUTABLE_ARGUMENT_ERROR(Args)                                                 \
    static_assert(                                                                                 \
        produces_mutable_argument_error<rome::fwd_delegate<void Args, rome::target_is_mandatory>>, \
        "");                                                                                       \
    static_assert(                                                                                 \
        produces_mutable_argument_error<rome::fwd_delegate<void Args, rome::target_is_expected>>,  \
        "");                                                                                       \
    static_assert(                                                                                 \
        produces_mutable_argument_error<rome::fwd_delegate<void Args, rome::target_is_optional>>,  \
        "")

TEST_WITH_VOID_RETURN_AND_ARGS(());

TEST_WITH_VOID_RETURN_AND_ARGS((int));

enum E : int { e_zero, e_one };
TEST_WITH_VOID_RETURN_AND_ARGS((E));

enum class EC : int { zero, one };
TEST_WITH_VOID_RETURN_AND_ARGS((EC));

union U {
    int i;
};
TEST_WITH_VOID_RETURN_AND_ARGS((U));

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
TEST_PRODUCES_MUTABLE_ARGUMENT_ERROR((C&));
TEST_WITH_VOID_RETURN_AND_ARGS((const C&));
TEST_WITH_VOID_RETURN_AND_ARGS((C &&));
TEST_PRODUCES_MUTABLE_ARGUMENT_ERROR((C*));
TEST_WITH_VOID_RETURN_AND_ARGS((const C*));

using TMemberObject = int C::*;
TEST_PRODUCES_MUTABLE_ARGUMENT_ERROR((TMemberObject));

using TMemberFunction = int (C::*)(int);
TEST_PRODUCES_MUTABLE_ARGUMENT_ERROR((TMemberFunction));

using TConstMemberFunction = int (C::*)(int) const;
TEST_PRODUCES_MUTABLE_ARGUMENT_ERROR((TConstMemberFunction));

using TArray = int[10];
TEST_PRODUCES_MUTABLE_ARGUMENT_ERROR((TArray));

using TArrayRef = int (&)[10];
TEST_PRODUCES_MUTABLE_ARGUMENT_ERROR((TArrayRef));

using TArrayPtr = int (*)[10];
TEST_PRODUCES_MUTABLE_ARGUMENT_ERROR((TArrayPtr));

using TFunctionRef = int (&)(int);
TEST_PRODUCES_MUTABLE_ARGUMENT_ERROR((TFunctionRef));

using TFunctionPtr = int (*)(int);
TEST_PRODUCES_MUTABLE_ARGUMENT_ERROR((TFunctionPtr));

TEST_CASE("rome::fwd_delegate - template parameter combinations") {
    CHECK_MESSAGE(
        true, "explicit template instantiations build with different possible type combinations");
    CHECK_MESSAGE(true, "check size of class");
    CHECK_MESSAGE(true, "check default constructor");
    CHECK_MESSAGE(true, "check copy constructor");
    CHECK_MESSAGE(true, "check copy assignment");
    CHECK_MESSAGE(true, "check move constructor");
    CHECK_MESSAGE(true, "check move assignment");
    CHECK_MESSAGE(true, "check destructor");
    CHECK_MESSAGE(true, "check construction from nullptr");
    CHECK_MESSAGE(true, "check assigning a nullptr");
    CHECK_MESSAGE(true, "check casting to bool");
    CHECK_MESSAGE(true, "check swapping with another fwd_delegate");
    CHECK_MESSAGE(true, "check calling with arguments");
}

TEST_SUITE_END();  // rome/event_delegate.hpp
