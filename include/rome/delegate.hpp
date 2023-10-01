//
// Project: C++ delegates
// File content:
//   - delegate<Ret(Args...), Behavior>
//   - fwd_delegate<void(Args...), Behavior>
//   - event_delegate<void(Args...)>
//   - command_delegate<void(Args...)>
// See the documentation for more information.
//
// The rome::delegate implementation is based on the article of
// Sergey Ryazanov: "The Impossibly Fast C++ Delegates", 18 Jul 2005
// https://www.codeproject.com/articles/11015/the-impossibly-fast-c-delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>

#include "detail/delegate_base.hpp"

namespace rome {

// Used as template argument for delegates to declare that it invoking an empty delegate is valid
// behavior.
struct target_is_optional;
// Used as template argument for delegates to declare that an assigned target is expected when the
// delegate is invoked and leads to an exception otherwise.
struct target_is_expected;
// Used as template argument for delegates to declare that an assigned target is mandatory when the
// delegate is invoked and thus the design ensures that always a delegate is assigned.
struct target_is_mandatory;

namespace detail {
    // Selects the invoker that is called for empty delegates based on the given behavior.
    template<typename Behavior>
    using select_empty_invoker_t =
        std::conditional_t<std::is_same<Behavior, target_is_optional>::value,
            delegate_base::no_call_invoker, delegate_base::exception_call_invoker>;

    // Always false. Used to mark invalid parameters in static_assert.
    template<typename>
    constexpr bool invalid = false;

    // Whether 'Behavior' is one of the valid types.
    template<typename Behavior>
    constexpr bool is_behavior = std::is_same<Behavior, target_is_expected>::value
                                 || std::is_same<Behavior, target_is_mandatory>::value
                                 || std::is_same<Behavior, target_is_optional>::value;

    // Whether 'Behavior' is valid for return type 'Ret'.
    template<typename Ret, typename Behavior>
    constexpr bool is_valid_behavior =
        !std::is_same<Behavior, target_is_optional>::value
        || (std::is_same<Behavior, target_is_optional>::value && std::is_same<Ret, void>::value);
}  // namespace detail

template<typename Signature, typename Behavior = target_is_expected>
class delegate {
    static_assert(detail::invalid<Signature>,
        "Invalid parameter 'Signature'. The template parameter "
        "'Signature' must be a valid function signature.");
};

template<typename Ret, typename... Args, typename Behavior>
class delegate<Ret(Args...), Behavior> {
    static_assert(detail::is_behavior<Behavior>,
        "Invalid parameter 'Behavior'. The template parameter 'Behavior' must either be empty or "
        "contain one of the types 'rome::target_is_optional', 'rome::target_is_expected' or "
        "'rome::target_is_mandatory'.");
    static_assert(detail::is_valid_behavior<Ret, Behavior>,
        "Return type coflicts with parameter 'Behavior'. The parameter 'Behavior' is only "
        "allowed to be 'rome::target_is_optional' if the return type is 'void'.");

    using delegate_base_type = detail::delegate_base::delegate_base<Ret(Args...),
        detail::select_empty_invoker_t<Behavior>>;

    delegate_base_type target_ = {};

    delegate(delegate_base_type&& target) noexcept : target_{std::move(target)} {
    }

  public:
    constexpr delegate() noexcept      = default;
    delegate(const delegate&) noexcept = delete;
    delegate(delegate&&) noexcept      = default;
    ~delegate()                        = default;

    constexpr delegate(std::nullptr_t) noexcept : delegate{} {
    }

    delegate& operator=(const delegate&) noexcept = delete;
    delegate& operator=(delegate&&) noexcept      = default;

    constexpr delegate& operator=(std::nullptr_t) noexcept {
        target_ = nullptr;
        return *this;
    }

    constexpr explicit operator bool() const noexcept {
        return target_.operator bool();
    }

    void swap(delegate& other) noexcept {
        target_.swap(other.target_);
    }

    Ret operator()(Args... args) const {
        return target_.operator()(static_cast<Args>(args)...);
    }

    // Creates a new delegate targeting the passed function or static member function.
    template<Ret (*pFunction)(Args...)>
    static constexpr delegate create() noexcept {
        return delegate{delegate_base_type::template create<pFunction>()};
    }

    // Creates a new delegate targeting the passed non-static member function.
    template<typename C, Ret (C::*pMethod)(Args...)>
    static delegate create(C& obj) noexcept {
        return delegate{delegate_base_type::template create<C, pMethod>(obj)};
    }

    // Creates a new delegate targeting the passed non-static const member function.
    template<typename C, Ret (C::*pMethod)(Args...) const>
    static delegate create(const C& obj) noexcept {
        return delegate{delegate_base_type::template create<C, pMethod>(obj)};
    }

    // Creates a new delegate targeting the passed functor and taking ownership of it.
    template<typename T>
    static delegate create(T&& functor) noexcept(
        noexcept(delegate_base_type::create(std::forward<T>(functor)))) {
        return delegate{delegate_base_type::template create(std::forward<T>(functor))};
    }
};

template<typename Ret, typename... Args>
class delegate<Ret(Args...), target_is_mandatory> {
    using delegate_base_type = detail::delegate_base::delegate_base<Ret(Args...),
        detail::delegate_base::exception_call_invoker>;

    delegate_base_type target_ = {};

    delegate(delegate_base_type&& target) noexcept : target_{std::move(target)} {
    }

  public:
    constexpr delegate() noexcept      = delete;
    delegate(const delegate&) noexcept = delete;
    delegate(delegate&&) noexcept      = default;
    ~delegate()                        = default;

    delegate& operator=(const delegate&) noexcept = delete;
    delegate& operator=(delegate&&) noexcept      = default;

    constexpr explicit operator bool() const noexcept {
        return target_.operator bool();
    }

    void swap(delegate& other) noexcept {
        target_.swap(other.target_);
    }

    Ret operator()(Args... args) const {
        return target_.operator()(static_cast<Args>(args)...);
    }

    // Creates a new delegate targeting the passed function or static member function.
    template<Ret (*pFunction)(Args...)>
    static constexpr delegate create() noexcept {
        return delegate{delegate_base_type::template create<pFunction>()};
    }

    // Creates a new delegate targeting the passed non-static member function.
    template<typename C, Ret (C::*pMethod)(Args...)>
    static delegate create(C& obj) noexcept {
        return delegate{delegate_base_type::template create<C, pMethod>(obj)};
    }

    // Creates a new delegate targeting the passed non-static const member function.
    template<typename C, Ret (C::*pMethod)(Args...) const>
    static delegate create(const C& obj) noexcept {
        return delegate{delegate_base_type::template create<C, pMethod>(obj)};
    }

    // Creates a new delegate targeting the passed functor and taking ownership of it.
    template<typename T>
    static delegate create(T&& functor) noexcept(
        noexcept(delegate_base_type::create(std::forward<T>(functor)))) {
        return delegate{delegate_base_type::template create(std::forward<T>(functor))};
    }
};

template<typename Sig, typename Behavior>
constexpr bool operator==(const delegate<Sig, Behavior>& lhs, std::nullptr_t) {
    return !lhs;
}

template<typename Sig, typename Behavior>
constexpr bool operator==(std::nullptr_t, const delegate<Sig, Behavior>& rhs) {
    return !rhs;
}

template<typename Sig, typename Behavior>
constexpr bool operator!=(const delegate<Sig, Behavior>& lhs, std::nullptr_t) {
    return static_cast<bool>(lhs);
}

template<typename Sig, typename Behavior>
constexpr bool operator!=(std::nullptr_t, const delegate<Sig, Behavior>& rhs) {
    return static_cast<bool>(rhs);
}

namespace detail {
    // Helper class for 'remove_member_pointer_t' below.
    template<typename T>
    struct remove_member_pointer_helper;

    template<typename T, typename C>
    struct remove_member_pointer_helper<T C::*> {
        using type = T;
    };

    // Returns the type of the object the given member object pointer can point to. Compile error if
    // no member object pointer was given.
    template<typename T>
    using remove_member_pointer_t =
        typename remove_member_pointer_helper<std::remove_cv_t<T>>::type;


    // Returns whether given function is const qualified.
    template<typename T>
    struct is_const_function : std::false_type {};

    template<typename Ret, typename... Args>
    struct is_const_function<Ret(Args...) const> : std::true_type {};
    template<typename Ret, typename... Args>
    struct is_const_function<Ret(Args...) const&> : std::true_type {};
    template<typename Ret, typename... Args>
    struct is_const_function<Ret(Args...) const&&> : std::true_type {};
    template<typename Ret, typename... Args>
    struct is_const_function<Ret(Args...) const volatile> : std::true_type {};
    template<typename Ret, typename... Args>
    struct is_const_function<Ret(Args...) const volatile&> : std::true_type {};
    template<typename Ret, typename... Args>
    struct is_const_function<Ret(Args...) const volatile&&> : std::true_type {};


    // Returns whether given type allows that data can be changed, directly or inderectly through
    // some kind of pointer or C-array. It cannot detect if a class allows to mutate data even if
    // declared const (limitation of the C++ language).
    template<typename T, typename = std::true_type>
    struct is_immutable;

    template<typename T>
    struct is_immutable<T,
        std::integral_constant<bool, std::is_void<T>::value || std::is_null_pointer<T>::value
                                         || std::is_function<T>::value>> : std::true_type {};

    template<typename T>
    struct is_immutable<T,
        std::integral_constant<bool, std::is_integral<T>::value || std::is_floating_point<T>::value
                                         || std::is_union<T>::value || std::is_enum<T>::value
                                         || std::is_class<T>::value>> : std::is_const<T>::type {};

    template<typename T>
    struct is_immutable<T, std::integral_constant<bool, std::is_array<T>::value>>
        : is_immutable<std::remove_all_extents_t<T>>::type {};

    template<typename T>
    struct is_immutable<T, std::integral_constant<bool, std::is_pointer<T>::value>>
        : std::integral_constant<bool,
              std::is_const<T>::value && is_immutable<std::remove_pointer_t<T>>::value> {};

    template<typename T>
    struct is_immutable<T, std::integral_constant<bool, std::is_member_object_pointer<T>::value>>
        : std::integral_constant<bool,
              std::is_const<T>::value && is_immutable<remove_member_pointer_t<T>>::value> {};

    template<typename T>
    struct is_immutable<T, std::integral_constant<bool, std::is_member_function_pointer<T>::value>>
        : is_const_function<remove_member_pointer_t<T>>::type {};


    // Returns whether given function argument can be considered immutable. The type of the argument
    // does not allow that the callee can change data of the caller, directly or inderectly through
    // some kind references, pointers or C-array. It cannot detect if a class allows to mutate data
    // even if declared const (limitation of the C++ language).
    template<typename Arg>
    constexpr auto is_immutable_argument() -> bool {
        using NoRef = std::remove_reference_t<Arg>;
        if (std::is_lvalue_reference<Arg>::value && !std::is_const<NoRef>::value
            && !std::is_null_pointer<NoRef>::value && !std::is_function<NoRef>::value) {
            return false;
        }
        // Moved or copied arguments are immutable in a sense as it is not possible to change the
        // data at the caller's side at this level.
        return is_immutable<const std::decay_t<Arg>>::value;
    }

    // Returns whether the function arguments 'Args...' can be considered immutable.
    template<typename... Args>
    constexpr bool are_immutable_arguments =
        std::is_same<std::integer_sequence<bool, true, is_immutable_argument<Args>()...>,
            std::integer_sequence<bool, is_immutable_argument<Args>()..., true>>::value;
}  // namespace detail

// same as rome::delegate but restricts to void return and enforces immutable arguments
template<typename Signature, typename Behavior = target_is_expected>
class fwd_delegate {
    static_assert(detail::invalid<Signature>,
        "Invalid parameter 'Signature'. The template parameter 'Signature' must be a valid "
        "function signature with return type 'void'. Consider using 'rome::delegate' if a non-void "
        "return type is needed.");
};

template<typename... Args, typename Behavior>
class fwd_delegate<void(Args...), Behavior> {
    static_assert(detail::is_behavior<Behavior>,
        "Invalid parameter 'Behavior'. The template parameter 'Behavior' must either be empty or "
        "contain one of the types 'rome::target_is_optional', 'rome::target_is_expected' or "
        "'rome::target_is_mandatory'.");
    static_assert(detail::are_immutable_arguments<Args...>,
        "Invalid mutable function argument in 'void(Args...)'. All function arguments of a "
        "'rome::fwd_delegate' must be immutable. The argument types shall prevent that the callee "
        "is able to modify passed data still owned by the caller. E.g. 'int&' is not allowed. "
        "'const int&' is allowed (readonly). 'int' and 'int&&' are also allowed (data owned by "
        "callee). Consider using 'rome::delegate' if mutable arguments are needed.");

    using delegate_base_type = detail::delegate_base::delegate_base<void(Args...),
        detail::select_empty_invoker_t<Behavior>>;

    delegate_base_type target_ = {};

    fwd_delegate(delegate_base_type&& target) noexcept : target_{std::move(target)} {
    }

  public:
    constexpr fwd_delegate() noexcept          = default;
    fwd_delegate(const fwd_delegate&) noexcept = delete;
    fwd_delegate(fwd_delegate&&) noexcept      = default;
    ~fwd_delegate()                            = default;

    constexpr fwd_delegate(std::nullptr_t) noexcept : fwd_delegate{} {
    }

    fwd_delegate& operator=(const fwd_delegate&) noexcept = delete;
    fwd_delegate& operator=(fwd_delegate&&) noexcept      = default;

    constexpr fwd_delegate& operator=(std::nullptr_t) noexcept {
        target_ = nullptr;
        return *this;
    }

    constexpr explicit operator bool() const noexcept {
        return target_.operator bool();
    }

    void swap(fwd_delegate& other) noexcept {
        target_.swap(other.target_);
    }

    void operator()(Args... args) const {
        target_.operator()(static_cast<Args>(args)...);
    }

    // Creates a new fwd_delegate targeting the passed function or static member function.
    template<void (*pFunction)(Args...)>
    static constexpr fwd_delegate create() noexcept {
        return fwd_delegate{delegate_base_type::template create<pFunction>()};
    }

    // Creates a new fwd_delegate targeting the passed non-static member function.
    template<typename C, void (C::*pMethod)(Args...)>
    static fwd_delegate create(C& obj) noexcept {
        return fwd_delegate{delegate_base_type::template create<C, pMethod>(obj)};
    }

    // Creates a new fwd_delegate targeting the passed non-static const member function.
    template<typename C, void (C::*pMethod)(Args...) const>
    static fwd_delegate create(const C& obj) noexcept {
        return fwd_delegate{delegate_base_type::template create<C, pMethod>(obj)};
    }

    // Creates a new fwd_delegate targeting the passed functor and taking ownership of it.
    template<typename T>
    static fwd_delegate create(T&& functor) noexcept(
        noexcept(delegate_base_type::create(std::forward<T>(functor)))) {
        return fwd_delegate{delegate_base_type::template create(std::forward<T>(functor))};
    }
};

template<typename... Args>
class fwd_delegate<void(Args...), target_is_mandatory> {
    static_assert(detail::are_immutable_arguments<Args...>,
        "Invalid mutable function argument in 'void(Args...)'. All function arguments of a "
        "'rome::fwd_delegate' must be immutable. The argument types shall prevent that the callee "
        "is able to modify passed data still owned by the caller. E.g. 'int&' is not allowed. "
        "'const int&' is allowed (readonly). 'int' and 'int&&' are also allowed (data owned by "
        "callee). Consider using 'rome::delegate' if mutable arguments are needed.");

    using delegate_base_type = detail::delegate_base::delegate_base<void(Args...),
        detail::delegate_base::exception_call_invoker>;

    delegate_base_type target_ = {};

    fwd_delegate(delegate_base_type&& target) noexcept : target_{std::move(target)} {
    }

  public:
    constexpr fwd_delegate() noexcept          = delete;
    fwd_delegate(const fwd_delegate&) noexcept = delete;
    fwd_delegate(fwd_delegate&&) noexcept      = default;
    ~fwd_delegate()                            = default;

    fwd_delegate& operator=(const fwd_delegate&) noexcept = delete;
    fwd_delegate& operator=(fwd_delegate&&) noexcept      = default;

    constexpr explicit operator bool() const noexcept {
        return target_.operator bool();
    }

    void swap(fwd_delegate& other) noexcept {
        target_.swap(other.target_);
    }

    void operator()(Args... args) const {
        target_.operator()(static_cast<Args>(args)...);
    }

    // Creates a new fwd_delegate targeting the passed function or static member function.
    template<void (*pFunction)(Args...)>
    static constexpr fwd_delegate create() noexcept {
        return fwd_delegate{delegate_base_type::template create<pFunction>()};
    }

    // Creates a new fwd_delegate targeting the passed non-static member function.
    template<typename C, void (C::*pMethod)(Args...)>
    static fwd_delegate create(C& obj) noexcept {
        return fwd_delegate{delegate_base_type::template create<C, pMethod>(obj)};
    }

    // Creates a new fwd_delegate targeting the passed non-static const member function.
    template<typename C, void (C::*pMethod)(Args...) const>
    static fwd_delegate create(const C& obj) noexcept {
        return fwd_delegate{delegate_base_type::template create<C, pMethod>(obj)};
    }

    // Creates a new fwd_delegate targeting the passed functor and taking ownership of it.
    template<typename T>
    static fwd_delegate create(T&& functor) noexcept(
        noexcept(delegate_base_type::create(std::forward<T>(functor)))) {
        return fwd_delegate{delegate_base_type::template create(std::forward<T>(functor))};
    }
};

template<typename Sig, typename Behavior>
constexpr bool operator==(const fwd_delegate<Sig, Behavior>& lhs, std::nullptr_t) {
    return !lhs;
}

template<typename Sig, typename Behavior>
constexpr bool operator==(std::nullptr_t, const fwd_delegate<Sig, Behavior>& rhs) {
    return !rhs;
}

template<typename Sig, typename Behavior>
constexpr bool operator!=(const fwd_delegate<Sig, Behavior>& lhs, std::nullptr_t) {
    return static_cast<bool>(lhs);
}

template<typename Sig, typename Behavior>
constexpr bool operator!=(std::nullptr_t, const fwd_delegate<Sig, Behavior>& rhs) {
    return static_cast<bool>(rhs);
}

template<typename Signature>
using event_delegate = fwd_delegate<Signature, target_is_optional>;

template<typename Signature>
using command_delegate = fwd_delegate<Signature, target_is_mandatory>;

}  // namespace rome
