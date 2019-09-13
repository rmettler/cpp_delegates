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

struct tgt_dyn_req;
struct tgt_stat_req;
struct tgt_opt;

namespace detail {
    namespace delegate {
        struct signature_error {};

        template<typename Signature>
        struct signature_error_ : signature_error {
            constexpr signature_error_() noexcept {
                static_assert(wrong<Signature>,
                    "Invalid template parameter. The first template parameter must be a function "
                    "signature of form 'Ret (Args...)'."
                    "E.g.: 'delgate<float (std::array<int,10>&)>'");
            }
        };

        template<typename Ret, typename TgtReq>
        constexpr bool checkTgtReq() {
            return std::is_same<TgtReq, rome::tgt_dyn_req>::value
                   || std::is_same<TgtReq, rome::tgt_stat_req>::value
                   || (std::is_same<TgtReq, rome::tgt_opt>::value
                       && std::is_same<Ret, void>::value);
        }

        struct target_requirement_error {};

        template<typename Ret, typename TgtReq>
        struct target_requirement_error_ : target_requirement_error {
            constexpr target_requirement_error_() noexcept {
                static_assert(wrong<Ret, TgtReq>,
                    "Invalid template parameter. The second template parameter TgtReq must either "
                    "be empty or contain one of the types rome::tgt_dyn_req, rome::tgt_stat_req or "
                    "rome::tgt_opt, where rome::tgt_opt is only valid if the return type Ret is "
                    "void.");
            }
        };
    };  // namespace delegate
};      // namespace detail

template<typename Signature, typename TgtReq = tgt_dyn_req>
class delegate : detail::delegate::signature_error_<Signature> {};

template<typename TgtReq, typename Ret, typename... Args>
class delegate<Ret(Args...), TgtReq>
    : std::conditional<detail::delegate::checkTgtReq<Ret, TgtReq>(), detail::ok,
          detail::delegate::target_requirement_error_<Ret, TgtReq>>::type {
    using target_type = detail::delegate::target_t<Ret(Args...)>;

    static constexpr target_type createEmptyTarget() {
        // TODO: depend no_call ot TgtReq
        return target_type{nullptr, target_type::no_call, target_type::no_delete};
    }

  public:
    // TODO: only if TgtReq matches and adapt the callee
    constexpr delegate() noexcept : target_{createEmptyTarget()} {
    }
    constexpr delegate(delegate&& orig) noexcept : target_{orig.target_} {
        orig.target_ = createEmptyTarget();
    }

    // TODO: only if TgtReq matches
    constexpr delegate(std::nullptr_t) noexcept : delegate{} {
    }

    constexpr delegate& operator=(delegate&& orig) noexcept {
        target_      = orig.target_;
        orig.target_ = createEmptyTarget();
    }
    // TODO: only if TgtReq matches
    constexpr delegate& operator=(std::nullptr_t) noexcept {
        *this = delegate{};
        return *this;
    }

    constexpr operator bool() const noexcept {
        return !!(*this);
    }
    constexpr bool operator!() const noexcept {
        // TODO: update this and check target_t! (depending on default)
        return target_.callee_ == createEmptyTarget().callee_;
    }

    constexpr bool operator==(const delegate& rhs) {
        return target_ == rhs.target_;
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

    target_type target_;
    alignas(sizeof(void*)) void* obj_ = nullptr;
    caller_type callee_               = null_callee;
};

template<typename Ret, typename... Args>
class delegate<Ret(Args...), tgt_stat_req>
    : std::conditional<detail::delegate::checkTgtReq<Ret, tgt_stat_req>(), detail::ok,
          detail::delegate::target_requirement_error_<Ret, tgt_stat_req>>::type {
  public:
    constexpr delegate(delegate&&) = default;
};

template<typename Sig, typename TgtReq>
constexpr bool operator==(const delegate<Sig, TgtReq>& lhs, std::nullptr_t) {
    return !lhs;
}

template<typename Sig, typename TgtReq>
constexpr bool operator==(std::nullptr_t, const delegate<Sig, TgtReq>& rhs) {
    return !rhs;
}

template<typename Sig, typename TgtReq>
constexpr bool operator!=(const delegate<Sig, TgtReq>& lhs, std::nullptr_t) {
    return lhs;
}

template<typename Sig, typename TgtReq>
constexpr bool operator!=(std::nullptr_t, const delegate<Sig, TgtReq>& rhs) {
    return rhs;
}

}  // namespace rome
