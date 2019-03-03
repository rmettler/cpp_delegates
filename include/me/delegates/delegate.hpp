//
// Project: delegates
// TODO: file description
//
// Based on the article of Sergey Ryazanov:
// "The Impossibly Fast C++ Delegates", 18 Jul 2005
// https://www.codeproject.com/articles/11015/the-impossibly-fast-c-delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
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
// TODO: split implementations and create a delegates.h file including all of
// them
// TODO: enhance the delegate factory
// TODO: is "non static member" the correct name? or are they only non static
// methods?

template <typename T> class delegate;

template <typename Ret, typename... Args> class delegate<Ret(Args...)> {
  public:
    using signature_type = Ret(Args...);
    using caller_type = Ret (*)(void *, Args...);

    constexpr delegate() = default;
    constexpr delegate(const delegate &orig) = default;
    constexpr delegate(delegate &&) = default;
    constexpr delegate(const std::nullptr_t &) : delegate() {}

    constexpr delegate &operator=(const delegate &orig) = default;
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
    constexpr bool operator!() const { return !isSet(); }
    constexpr operator bool() const { return isSet(); }

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

    void *obj_ = nullptr;
    caller_type caller_ = nullptr;
};

template <typename T, T t, typename Enable = void> struct delegate_factory;

template <typename C, typename Ret, typename... Args, Ret (C::*pMem)(Args...)>
struct delegate_factory<Ret (C::*)(Args...), pMem> {
    constexpr static auto create(C *obj)
    {
        return delegate<Ret(
            Args...)>::template create_from_non_static_member<C, pMem>(obj);
    }
};

template <typename C, typename Ret, typename... Args,
          Ret (C::*pMem)(Args...) const>
struct delegate_factory<Ret (C::*)(Args...) const, pMem> {
    constexpr static auto create(C const *obj)
    {
        return delegate<Ret(Args...)>::
            template create_from_non_static_const_member<C, pMem>(obj);
    }
};

template <typename Ret, typename... Args, Ret (*pMem)(Args...)>
struct delegate_factory<Ret (*)(Args...), pMem> {
    constexpr static auto create()
    {
        return delegate<Ret(Args...)>::template create_from_function<pMem>();
    }
};

template <typename T, T t>
constexpr auto make_delegate = delegate_factory<T, t>::create;

} // namespace delegates
} // namespace me
