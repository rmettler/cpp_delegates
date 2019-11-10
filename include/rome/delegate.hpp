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
}  // namespace detail

template<typename Signature, typename ExpectedBehavior = target_is_expected>
class delegate;

template<typename Ret, typename... Args, typename ExpectedBehavior>
class delegate<Ret(Args...), ExpectedBehavior>
    : std::conditional<detail::delegateHasValidExpectedBehavior<Ret, ExpectedBehavior>(),
          detail::ok, detail::invalid_delegate_expected_behavior_<Ret, ExpectedBehavior>>::type {
  public:
    // TODO: ev ist es einfacher, für target_is_mandatory ganz zu spezialisieren
    template<typename C                                                     = ExpectedBehavior,
        std::enable_if_t<!std::is_same<C, target_is_mandatory>::value, int> = 0>
    constexpr delegate() noexcept {
    }
    constexpr delegate(const delegate&) noexcept = delete;
    constexpr delegate(delegate&& orig) noexcept = default;

    template<typename C                                                     = ExpectedBehavior,
        std::enable_if_t<!std::is_same<C, target_is_mandatory>::value, int> = 0>
    constexpr delegate(std::nullptr_t) noexcept : delegate{} {
    }

    constexpr delegate& operator=(const delegate&) = delete;
    constexpr delegate& operator=(delegate&& orig) = default;

    template<typename C                                                     = ExpectedBehavior,
        std::enable_if_t<!std::is_same<C, target_is_mandatory>::value, int> = 0>
    constexpr delegate& operator=(std::nullptr_t) noexcept {
        *this = delegate{};
        return *this;
    }

    constexpr operator bool() const noexcept {
        // TODO: update this and check target_t! (depending on default)
        return callee_ != null_callee;
    }

    void swap(delegate&) noexcept {}

    inline Ret operator()(Args... args) const {
        return (*callee_)(obj_, std::forward<Args>(args)...);
    }

    template<Ret (*pFunction)(Args...)>
    static constexpr delegate create() noexcept {
        return delegate{nullptr, &function_call<pFunction>};
    }

    template<typename C, Ret (C::*pMethod)(Args...)>
    static delegate create(C& obj) noexcept {
        return delegate{&obj, &method_call<C, pMethod>};
    }

    template<typename C, Ret (C::*pMethod)(Args...) const>
    static delegate create(const C& obj) noexcept {
        return delegate{const_cast<C*>(&obj), &const_method_call<C, pMethod>};
    }

    template<typename T>
    static constexpr delegate create(T& functor) noexcept {
        return delegate{&functor, &functor_call<T>};
    }

    template<typename T>
    static constexpr delegate create(const T& functor) noexcept {
        return delegate{const_cast<T*>(&functor), &const_functor_call<T>};
    }

  private:
    constexpr delegate(void* obj, Ret (*callee)(void*, Args...)) noexcept
        : obj_{obj}, callee_{callee} {
    }

    template<typename C, Ret (C::*pMethod)(Args...)>
    static Ret method_call(void* obj, Args... args) {
        return (static_cast<C*>(obj)->*pMethod)(args...);
    }

    template<typename C, Ret (C::*pMethod)(Args...) const>
    static Ret const_method_call(void* obj, Args... args) {
        return (static_cast<const C*>(obj)->*pMethod)(args...);
    }

    template<Ret (*pFunction)(Args...)>
    static constexpr Ret function_call(void*, Args... args) {
        return (*pFunction)(args...);
    }

    template<typename T>
    static constexpr Ret functor_call(void* obj, Args... args) {
        return static_cast<T*>(obj)->operator()(args...);
    }

    template<typename T>
    static constexpr Ret const_functor_call(void* obj, Args... args) {
        return static_cast<const T*>(obj)->operator()(args...);
    }

    static constexpr void null_call(void*, Args...) {
    }

    template<typename T, typename std::enable_if_t<!std::is_same<T, void>::value, int> = 0>
    static constexpr auto get_null_callee() {
        return nullptr;
    }

    template<typename T, typename std::enable_if_t<std::is_same<T, void>::value, int> = 0>
    static constexpr auto get_null_callee() {
        return null_call;
    }

    static constexpr auto null_callee = get_null_callee<Ret>();

    using caller_type = Ret (*)(void*, Args...);

    alignas(sizeof(void*)) void* obj_ = nullptr;
    caller_type callee_               = null_callee;
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
