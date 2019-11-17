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

// Used as template argument for delegates to declare that it is valid behavior invoking an empty
// delegate.
struct target_is_optional;
// Used as template argument for delegates to declare that an assigned target is expected when the
// delegate is invoked and leads to an exception otherwise.
struct target_is_expected;
// Used as template argument for delegates to declare that an assigned target is mandatory when the
// delegate is invoke and thus the design ensures that always a delegate is assigned.
struct target_is_mandatory;

namespace detail {
    template<typename Ret, typename ExpectedBehavior>
    constexpr bool delegateHasValidExpectedBehavior() {
        return std::is_same<ExpectedBehavior, rome::target_is_expected>::value
               || std::is_same<ExpectedBehavior, rome::target_is_mandatory>::value
               || (std::is_same<ExpectedBehavior, rome::target_is_optional>::value
                   && std::is_same<Ret, void>::value);
    }

    struct invalid_delegate_expected_behavior {};

    template<typename Ret, typename ExpectedBehavior>
    struct invalid_delegate_expected_behavior_ : invalid_delegate_expected_behavior {
        constexpr invalid_delegate_expected_behavior_() noexcept {
            static_assert(wrong<Ret, ExpectedBehavior>,
                "Invalid template parameter. The second template parameter 'ExpectedBehavior' must "
                "either be empty or contain one of the types 'rome::target_is_optional', "
                "'rome::target_is_expected' or 'rome::target_is_mandatory', where "
                "'rome::target_is_optional' is only valid if the return type 'Ret' is 'void'.");
        }
    };

    template<typename ExpectedBehavior>
    struct select_empty_invoker_for;

    template<>
    struct select_empty_invoker_for<target_is_optional> {
        using type = detail::delegate_base::no_call_invoker;
    };

    template<>
    struct select_empty_invoker_for<target_is_expected> {
        using type = detail::delegate_base::exception_call_invoker;
    };
}  // namespace detail

template<typename Signature, typename ExpectedBehavior = target_is_expected>
class delegate;

template<typename Ret, typename... Args, typename ExpectedBehavior>
class delegate<Ret(Args...), ExpectedBehavior>
    : std::conditional<detail::delegateHasValidExpectedBehavior<Ret, ExpectedBehavior>(),
          detail::ok, detail::invalid_delegate_expected_behavior_<Ret, ExpectedBehavior>>::type {
  private:
    using base_delegate_type = detail::delegate_base::delegate_base<Ret(Args...),
        typename detail::select_empty_invoker_for<ExpectedBehavior>::type>;

    base_delegate_type target_ = {};

    delegate(decltype(target_)&& target) noexcept : target_{std::move(target)} {
    }

  public:
    constexpr delegate() noexcept      = default;
    delegate(const delegate&) noexcept = delete;
    delegate(delegate&& orig) noexcept = default;
    ~delegate()                        = default;

    constexpr delegate(std::nullptr_t) noexcept : delegate{} {
    }

    delegate& operator=(const delegate&) noexcept = delete;
    delegate& operator=(delegate&& orig) noexcept = default;

    constexpr delegate& operator=(std::nullptr_t) noexcept {
        target_ = nullptr;
        return *this;
    }

    constexpr explicit operator bool() const noexcept {
        return target_;
    }

    void swap(delegate& other) noexcept {
        target_.swap(other.target_);
    }

    inline Ret operator()(Args... args) const {
        return target_.operator()(std::forward<Args>(args)...);
    }

    template<Ret (*pFunction)(Args...)>
    static constexpr delegate create() noexcept {
        return delegate{decltype(target_)::create<pFunction>()};
    }

    template<typename C, Ret (C::*pMethod)(Args...)>
    static delegate create(C& obj) noexcept {
        return delegate{decltype(target_)::create<C, pMethod>(obj)};
    }

    template<typename C, Ret (C::*pMethod)(Args...) const>
    static delegate create(const C& obj) noexcept {
        return delegate{decltype(target_)::create<C, pMethod>(obj)};
    }

    template<typename T>
    static delegate create(T functor) noexcept(noexcept(decltype(target_)::create(functor))) {
        return delegate{decltype(target_)::create(functor))};
    }
};

template<typename Ret, typename... Args>
class delegate<Ret(Args...), target_is_mandatory>
    : std::conditional<detail::delegateHasValidExpectedBehavior<Ret, ExpectedBehavior>(),
          detail::ok, detail::invalid_delegate_expected_behavior_<Ret, ExpectedBehavior>>::type {
  private:
    using base_delegate_type = detail::delegate_base::delegate_base<Ret(Args...),
        detail::delegate_base::exception_call_invoker>;

    base_delegate_type target_ = {};

    delegate(decltype(target_)&& target) noexcept : target_{std::move(target)} {
    }

  public:
    constexpr delegate() noexcept      = delete;
    delegate(const delegate&) noexcept = delete;
    delegate(delegate&& orig) noexcept = default;
    // TODO: orig should be emptied (might do it automatically?)
    ~delegate() = default;

    delegate& operator=(const delegate&) noexcept = delete;
    delegate& operator=(delegate&& orig) noexcept = default;
    // TODO: orig should be emptied (might do it automatically?)

    constexpr explicit operator bool() const noexcept {
        return target_;
    }

    void swap(delegate& other) noexcept {
        target_.swap(other.target_);
    }

    inline Ret operator()(Args... args) const {
        return target_.operator()(std::forward<Args>(args)...);
    }

    template<Ret (*pFunction)(Args...)>
    static constexpr delegate create() noexcept {
        return delegate{decltype(target_)::create<pFunction>()};
    }

    template<typename C, Ret (C::*pMethod)(Args...)>
    static delegate create(C& obj) noexcept {
        return delegate{decltype(target_)::create<C, pMethod>(obj)};
    }

    template<typename C, Ret (C::*pMethod)(Args...) const>
    static delegate create(const C& obj) noexcept {
        return delegate{decltype(target_)::create<C, pMethod>(obj)};
    }

    template<typename T>
    static delegate create(T functor) noexcept(noexcept(decltype(target_)::create(functor))) {
        return delegate{decltype(target_)::create(functor))};
    }
};

template<typename Sig, typename ExpectedBehavior>
constexpr bool operator==(const delegate<Sig, ExpectedBehavior>& lhs, std::nullptr_t) {
    return !lhs;
}

template<typename Sig, typename ExpectedBehavior>
constexpr bool operator==(std::nullptr_t, const delegate<Sig, ExpectedBehavior>& rhs) {
    return !rhs;
}

template<typename Sig, typename ExpectedBehavior>
constexpr bool operator!=(const delegate<Sig, ExpectedBehavior>& lhs, std::nullptr_t) {
    return lhs;
}

template<typename Sig, typename ExpectedBehavior>
constexpr bool operator!=(std::nullptr_t, const delegate<Sig, ExpectedBehavior>& rhs) {
    return rhs;
}

}  // namespace rome
