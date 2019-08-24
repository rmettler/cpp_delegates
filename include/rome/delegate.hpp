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

#include <cstddef>
#include <type_traits>
#include <utility>

#include "detail/assertions.hpp"

namespace rome {

namespace detail {
    namespace delegate {
        struct signature_error {};

        template<typename Signature>
        struct signature_error_ : signature_error {
            constexpr signature_error_() {
                static_assert(wrong<Signature>::value,
                    "Invalid template parameter. The first template parameter must be a function "
                    "signature of form 'Ret (Args...)'."
                    "E.g.: 'delgate<float (std::array<int,10>&)>'");
            }
        };
    };  // namespace delegate
};      // namespace detail

template<typename Signature, bool TargetRequired = true>
class delegate : detail::delegate::signature_error_<Signature> {};

template<bool TargetRequired, typename Ret, typename... Args>
class delegate<Ret(Args...), TargetRequired> {
  public:
    constexpr delegate()                = default;
    constexpr delegate(const delegate&) = default;
    constexpr delegate(delegate&&)      = default;
    constexpr delegate(std::nullptr_t) : delegate{} {
    }

    constexpr delegate& operator=(const delegate&) = default;
    constexpr delegate& operator=(delegate&&) = default;
    constexpr delegate& operator              =(std::nullptr_t) {
        *this = std::move(delegate{});
        return *this;
    }

    constexpr bool isLinked() const {
        return callee_ != null_callee;
    }
    constexpr operator bool() const {
        return isLinked();
    }
    constexpr bool operator!() const {
        return !isLinked();
    }

    constexpr bool operator==(const delegate& rhs) {
        return (obj_ == rhs.obj_) && (callee_ == rhs.callee_);
    }

    constexpr bool operator!=(const delegate& rhs) {
        return !(*this == rhs);
    }

    constexpr Ret operator()(Args... args) const {
        return callee_(obj_, std::forward<Args>(args)...);
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

template<typename Sig, bool TR>
constexpr bool operator==(const delegate<Sig, TR>& lhs, std::nullptr_t) {
    return !lhs;
}

template<typename Sig, bool TR>
constexpr bool operator==(std::nullptr_t, const delegate<Sig, TR>& rhs) {
    return !rhs;
}

template<typename Sig, bool TR>
constexpr bool operator!=(const delegate<Sig, TR>& lhs, std::nullptr_t) {
    return !(lhs == nullptr);
}

template<typename Sig, bool TR>
constexpr bool operator!=(std::nullptr_t, const delegate<Sig, TR>& rhs) {
    return !(rhs == nullptr);
}

}  // namespace rome
