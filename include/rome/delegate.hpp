//
// Project: C++ delegates
// File content:
//   - delegate<Ret (Args...)>
//   - delegate<void (Args...)>
// See the documentation for more information.
//
// The delegates::delegate implementation is based on the article of
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

#include "bad_delegate_call.hpp"
#include "detail/assertions.hpp"
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
    struct invalid_delegate_expected_behavior {};

    template<typename Ret, typename Behavior>
    struct invalid_delegate_expected_behavior_ : invalid_delegate_expected_behavior {
        constexpr invalid_delegate_expected_behavior_() noexcept {
            static_assert(wrong<Ret, Behavior>,
                "Invalid template parameter. The second template parameter 'Behavior' must "
                "either be empty or contain one of the types 'rome::target_is_optional', "
                "'rome::target_is_expected' or 'rome::target_is_mandatory', where "
                "'rome::target_is_optional' is only valid if the return type 'Ret' is 'void'.");
        }
    };

    // TODO: change name to expected_behavior_delegate_param_invalid!
    // TODO: also cleanup unused helper macros for constexpr stuff
    template<typename Ret, typename Behavior>
    struct delegate_helper {
        template<typename... Args>
        using delegate_base_type =
            delegate_base::delegate_base<Ret(Args...), delegate_base::invalid_invoker>;
        using assert_template_params = invalid_delegate_expected_behavior_<Ret, Behavior>;
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

}  // namespace rome
