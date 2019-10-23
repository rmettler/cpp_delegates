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

// Used as template argument for delegates to declare that
// TODO descriptions
struct target_optional;
struct target_expected;
struct target_enforced;

namespace detail {
    template<typename Ret, typename Character>
    constexpr bool delegateHasValidCharacter() {
        return std::is_same<Character, rome::target_expected>::value
               || std::is_same<Character, rome::target_enforced>::value
               || (std::is_same<Character, rome::target_optional>::value
                   && std::is_same<Ret, void>::value);
    }

    struct invalid_delegate_character {};

    template<typename Ret, typename Character>
    struct invalid_delegate_character_ : invalid_delegate_character {
        constexpr invalid_delegate_character_() noexcept {
            static_assert(wrong<Ret, Character>,
                "Invalid template parameter. The second template parameter 'Character' must "
                "either be empty or contain one of the types 'rome::target_optional', "
                "'rome::target_expected' or 'rome::target_enforced', where "
                "'rome::target_optional' is only valid if the return type 'Ret' is 'void'.");
        }
    };
};  // namespace detail

template<typename Signature, typename Character = target_expected>
class delegate;

template<typename Character, typename Ret, typename... Args>
class delegate<Ret(Args...), Character>
    : std::conditional<detail::delegateHasValidCharacter<Ret, Character>(), detail::ok,
          detail::invalid_delegate_character_<Ret, Character>>::type {
  public:
    // TODO: ev ist es einfacher, für target_enforced ganz zu spezialisieren
    template<typename C                                                 = Character,
        std::enable_if_t<!std::is_same<C, target_enforced>::value, int> = 0>
    constexpr delegate() noexcept {
    }
    constexpr delegate(const delegate&) noexcept = delete;
    constexpr delegate(delegate&& orig) noexcept = default;

    template<typename C                                                 = Character,
        std::enable_if_t<!std::is_same<C, target_enforced>::value, int> = 0>
    constexpr delegate(std::nullptr_t) noexcept : delegate{} {
    }

    constexpr delegate& operator=(const delegate&) = delete;
    constexpr delegate& operator=(delegate&& orig) = default;
    
    template<typename C                                                 = Character,
        std::enable_if_t<!std::is_same<C, target_enforced>::value, int> = 0>
    constexpr delegate& operator=(std::nullptr_t) noexcept {
        *this = delegate{};
        return *this;
    }

    constexpr operator bool() const noexcept {
        return !!(*this);
    }
    constexpr bool operator!() const noexcept {
        // TODO: update this and check target_t! (depending on default)
        return false;
    }

    // TODO: dieser operator macht keinen Sinn mehr, da vergleich wegen Heap pointer nicht
    //       mehr sauber möglich ist und vergleichen ohne kopieren zu können witzlos ist.
    constexpr bool operator==(const delegate& rhs) {
        // TODO target_ == rhs.target_
        return false;
    }

    constexpr bool operator!=(const delegate& rhs) {
        return !(*this == rhs);
    }

    inline Ret operator()(Args... args) const {
        return (*callee_)(obj_, std::forward<Args>(args)...);
    }

    template<Ret (*pFunction)(Args...)>
    static constexpr delegate create() {
        delegate d;
        d.obj_    = nullptr;
        d.callee_ = &function_call<pFunction>;
        return d;
    }

    template<typename C, Ret (C::*pMethod)(Args...)>
    static constexpr delegate create(C& obj) {
        delegate d;
        d.obj_    = &obj;
        d.callee_ = &method_call<C, pMethod>;
        return d;
    }

    template<typename C, Ret (C::*pMethod)(Args...) const>
    static constexpr delegate create(const C& obj) {
        delegate d;
        d.obj_    = const_cast<C*>(&obj);
        d.callee_ = &const_method_call<C, pMethod>;
        return d;
    }

    template<typename T>
    static constexpr delegate create(T& functor) {
        delegate d;
        d.obj_    = &functor;
        d.callee_ = &functor_call<T>;
        return d;
    }

    template<typename T>
    static constexpr delegate create(const T& functor) {
        delegate d;
        d.obj_    = const_cast<T*>(&functor);
        d.callee_ = &const_functor_call<T>;
        return d;
    }

  private:
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

template<typename Sig, typename Character>
constexpr bool operator==(const delegate<Sig, Character>& lhs, std::nullptr_t) {
    return !lhs;
}

template<typename Sig, typename Character>
constexpr bool operator==(std::nullptr_t, const delegate<Sig, Character>& rhs) {
    return !rhs;
}

template<typename Sig, typename Character>
constexpr bool operator!=(const delegate<Sig, Character>& lhs, std::nullptr_t) {
    return lhs;
}

template<typename Sig, typename Character>
constexpr bool operator!=(std::nullptr_t, const delegate<Sig, Character>& rhs) {
    return rhs;
}

}  // namespace rome
