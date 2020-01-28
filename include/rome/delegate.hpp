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
#include <cassert>
#include <cstddef>
#include <exception>
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
    struct ok {};

    template<typename...>
    struct wrong_t : std::false_type {};

    template<typename... T>
    constexpr bool wrong = wrong_t<T...>::value;

    struct bad_delegate_template_param {};

    template<typename Ret, typename Behavior>
    struct bad_delegate_template_param_ : bad_delegate_template_param {
        constexpr bad_delegate_template_param_() noexcept {
            static_assert(wrong<Ret, Behavior>,
                "Invalid template parameter. The second template parameter 'Behavior' must "
                "either be empty or contain one of the types 'rome::target_is_optional', "
                "'rome::target_is_expected' or 'rome::target_is_mandatory', where "
                "'rome::target_is_optional' is only valid if the return type 'Ret' is 'void'.");
        }
    };

    template<typename Ret, typename Behavior>
    struct delegate_helper {
        template<typename... Args>
        using delegate_base_type =
            delegate_base::delegate_base<Ret(Args...), delegate_base::invalid_invoker>;
        using assert_template_params = bad_delegate_template_param_<Ret, Behavior>;
    };

    template<>
    struct delegate_helper<void, target_is_optional> {
        template<typename... Args>
        using delegate_base_type =
            delegate_base::delegate_base<void(Args...), delegate_base::no_call_invoker>;
        using assert_template_params = ok;
    };

    template<typename Ret>
    struct delegate_helper<Ret, target_is_expected> {
        template<typename... Args>
        using delegate_base_type =
            delegate_base::delegate_base<Ret(Args...), delegate_base::exception_call_invoker>;
        using assert_template_params = ok;
    };
}  // namespace detail

template<typename Signature, typename Behavior = target_is_expected>
class delegate;

template<typename Ret, typename... Args, typename Behavior>
class delegate<Ret(Args...), Behavior>
    : private detail::delegate_helper<Ret, Behavior>::assert_template_params {
  private:
    using delegate_base_type =
        typename detail::delegate_helper<Ret, Behavior>::template delegate_base_type<Args...>;

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
    delegate& operator=(delegate&&) noexcept = default;

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
        return target_.operator()(std::forward<Args>(args)...);
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
  private:
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
    delegate& operator=(delegate&&) noexcept = default;

    constexpr explicit operator bool() const noexcept {
        return target_.operator bool();
    }

    void swap(delegate& other) noexcept {
        target_.swap(other.target_);
    }

    Ret operator()(Args... args) const {
        return target_.operator()(std::forward<Args>(args)...);
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
    struct bad_delegate_template_param_with_mutable_arguments {};

    template<typename... Args>
    struct bad_delegate_template_param_with_mutable_arguments_
        : bad_delegate_template_param_with_mutable_arguments {
        constexpr bad_delegate_template_param_with_mutable_arguments_() noexcept {
            static_assert(wrong<Args...>,
                "Invalid template parameter. The first template parameter 'Signature' must be "
                "of type 'void(Args...)' and all arguments 'Args...' must be of immutable type. "
                "E.g.: 'fwd_delegate<void (int, int&&, const std::array<int,10>&, const "
                "int*)>'");
        }
    };

    template<typename T>
    constexpr bool is_value() {
        return std::is_null_pointer<T>::value || std::is_integral<T>::value
               || std::is_floating_point<T>::value || std::is_enum<T>::value
               || std::is_union<T>::value || std::is_class<T>::value;
    }

    template<typename T>
    constexpr bool is_referenced_type_a_const_value() {
        if (is_value<T>()) {
            return std::is_const<T>::value;
        }
        if (std::is_pointer<T>::value) {
            using TT = typename std::remove_pointer<T>::type;
            return is_referenced_type_a_const_value<TT>();
        }
        return false;
    }

    template<typename T>
    constexpr bool is_copied_moved_or_const_referenced_value() {
        if (is_value<T>()) {
            return true;
        }
        if (std::is_rvalue_reference<T>::value) {
            using TT = typename std::remove_reference<T>::type;
            return is_copied_moved_or_const_referenced_value<TT>();
        }
        if (std::is_lvalue_reference<T>::value) {
            using TT = typename std::remove_reference<T>::type;
            return is_referenced_type_a_const_value<TT>();
        }
        return is_referenced_type_a_const_value<T>();
    }

    struct fwd_delegate_params_ok {};

    template<typename... Args>
    using fwd_delegate_assert_immutable_arguments = std::conditional_t<
        std::is_same<
            std::integer_sequence<bool, true, is_copied_moved_or_const_referenced_value<Args>()...>,
            std::integer_sequence<bool, is_copied_moved_or_const_referenced_value<Args>()...,
                true>>::value,
        fwd_delegate_params_ok, bad_delegate_template_param_with_mutable_arguments_<Args...>>;
}  // namespace detail

// same as rome::delegate but restricts to void return and enforces immutable arguments
template<typename Signature, typename Behavior = target_is_expected>
class fwd_delegate;

template<typename... Args, typename Behavior>
class fwd_delegate<void(Args...), Behavior>
    : private detail::delegate_helper<void, Behavior>::assert_template_params,
      private detail::fwd_delegate_assert_immutable_arguments<Args...> {
  private:
    using delegate_base_type =
        typename detail::delegate_helper<void, Behavior>::template delegate_base_type<Args...>;

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
    fwd_delegate& operator=(fwd_delegate&&) noexcept = default;

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
        return target_.operator()(std::forward<Args>(args)...);
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
class fwd_delegate<void(Args...), target_is_mandatory>
    : private detail::fwd_delegate_assert_immutable_arguments<Args...> {
  private:
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
    fwd_delegate& operator=(fwd_delegate&&) noexcept = default;

    constexpr explicit operator bool() const noexcept {
        return target_.operator bool();
    }

    void swap(fwd_delegate& other) noexcept {
        target_.swap(other.target_);
    }

    void operator()(Args... args) const {
        return target_.operator()(std::forward<Args>(args)...);
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
