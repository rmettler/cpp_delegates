//
// Project: delegates
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

namespace me {
namespace delegates {
// TODO: add a proper .clang-format!!!
// TODO: check whether is function, member, static or non-static
//      std::is_function -> all functions
//      std::is_member_pointer, is_member_object_pointer,
//      is_member_function_pointer
// TODO: compare with other implementations
// TODO: is "non static member" the correct name? or are they only non static
// methods?

template <typename T> class delegate;

template <typename Ret, typename... Args> class delegate<Ret(Args...)> {
  public:
    constexpr delegate() = default;
    constexpr delegate(const delegate &) = default;
    constexpr delegate(delegate &&) = default;
    constexpr delegate(const std::nullptr_t &) : delegate() {}

    constexpr delegate &operator=(const delegate &) = default;
    constexpr delegate &operator=(delegate &&) = default;
    constexpr delegate &operator=(const std::nullptr_t &)
    {
        obj_ = nullptr;
        caller_ = nullptr;
    }

    constexpr Ret operator()(Args... args) const
    {
        return caller_(obj_, args...);
    }

    constexpr bool isSet() const { return caller_ != nullptr; }
    constexpr operator bool() const { return isSet(); }
    constexpr bool operator!() const { return !isSet(); }

    template <typename C, Ret (C::*pMethod)(Args...)>
    constexpr static delegate create_from_non_static_member(C *obj)
    {
        delegate d;
        d.obj_ = obj;
        d.caller_ = (obj != nullptr) ? &method_call<C, pMethod> : nullptr;
        return d;
    }

    template <typename C, Ret (C::*pMethod)(Args...) const>
    constexpr static delegate create_from_non_static_const_member(C const *obj)
    {
        delegate d;
        d.obj_ = const_cast<C *>(obj);
        d.caller_ = (obj != nullptr) ? &const_method_call<C, pMethod> : nullptr;
        return d;
    }

    template <Ret (*pFunction)(Args...)>
    constexpr static delegate create_from_function()
    {
        delegate d;
        d.obj_ = nullptr;
        d.caller_ = &function_call<pFunction>;
        return d;
    }

  private:
    template <typename C, Ret (C::*pMethod)(Args...)>
    constexpr static Ret method_call(void *obj, Args... args)
    {
        return (static_cast<C *>(obj)->*pMethod)(args...);
    }

    template <typename C, Ret (C::*pMethod)(Args...) const>
    constexpr static Ret const_method_call(void *obj, Args... args)
    {
        return (static_cast<C const *>(obj)->*pMethod)(args...);
    }

    template <Ret (*pFunction)(Args...)>
    constexpr static Ret function_call(void *, Args... args)
    {
        return (*pFunction)(args...);
    }

    using caller_type = Ret (*)(void *, Args...);
    void *obj_ = nullptr;
    caller_type caller_ = nullptr;
};

template <typename... Args> class delegate<void(Args...)> {
  public:
    constexpr delegate() = default;
    constexpr delegate(const delegate &orig) = default;
    constexpr delegate(delegate &&) = default;
    constexpr delegate(const std::nullptr_t &) : delegate() {}

    constexpr delegate &operator=(const delegate &orig) = default;
    constexpr delegate &operator=(delegate &&) = default;
    constexpr delegate &operator=(const std::nullptr_t &)
    {
        obj_ = nullptr;
        caller_ = null_call;
    }

    constexpr void operator()(Args... args) const { caller_(obj_, args...); }

    constexpr bool isSet() const { return caller_ != nullptr; }
    constexpr bool operator!() const { return !isSet(); }
    constexpr operator bool() const { return isSet(); }

    template <typename C, void (C::*pMethod)(Args...)>
    constexpr static delegate create_from_non_static_member(C *obj)
    {
        delegate d;
        d.obj_ = obj;
        d.caller_ = (obj != nullptr) ? &method_call<C, pMethod> : null_call;
        return d;
    }

    template <typename C, void (C::*pMethod)(Args...) const>
    constexpr static delegate create_from_non_static_const_member(C const *obj)
    {
        delegate d;
        d.obj_ = const_cast<C *>(obj);
        d.caller_ =
            (obj != nullptr) ? &const_method_call<C, pMethod> : null_call;
        return d;
    }

    template <void (*pFunction)(Args...)>
    constexpr static delegate create_from_function()
    {
        delegate d;
        d.obj_ = nullptr;
        d.caller_ = &function_call<pFunction>;
        return d;
    }

  private:
    template <typename C, void (C::*pMethod)(Args...)>
    constexpr static void method_call(void *obj, Args... args)
    {
        (static_cast<C *>(obj)->*pMethod)(args...);
    }

    template <typename C, void (C::*pMethod)(Args...) const>
    constexpr static void const_method_call(void *obj, Args... args)
    {
        (static_cast<C const *>(obj)->*pMethod)(args...);
    }

    template <void (*pFunction)(Args...)>
    constexpr static void function_call(void *, Args... args)
    {
        (*pFunction)(args...);
    }

    constexpr static void null_call(void *, Args...) {}

    using caller_type = void (*)(void *, Args...);
    void *obj_ = nullptr;
    caller_type caller_ = null_call;
};

} // namespace delegates
} // namespace me
