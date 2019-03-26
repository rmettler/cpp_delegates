//
// Project: C++ delegates
// File content:
//   - make_delegate<..>(..)
// See the documentation for more information.
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "delegate.hpp"

namespace rome {
namespace delegates {

namespace detail {
template <typename T, T t> struct delegate_factory;

template <typename C, typename Ret, typename... Args, Ret (C::*pMem)(Args...)>
struct delegate_factory<Ret (C::*)(Args...), pMem> {
    constexpr static auto create(C *obj)
    {
        return delegate<Ret(
            Args...)>::template createFromNonStaticMemberFunction<C, pMem>(obj);
    }
};

template <typename C, typename Ret, typename... Args,
          Ret (C::*pMem)(Args...) const>
struct delegate_factory<Ret (C::*)(Args...) const, pMem> {
    constexpr static auto create(C const *obj)
    {
        return delegate<Ret(Args...)>::
            template createFromNonStaticConstMemberFunction<C, pMem>(obj);
    }
};

template <typename Ret, typename... Args, Ret (*pMem)(Args...)>
struct delegate_factory<Ret (*)(Args...), pMem> {
    constexpr static auto create()
    {
        return delegate<Ret(Args...)>::template createFromFunction<pMem>();
    }
};
} // namespace detail

template <typename T, T t>
constexpr auto make_delegate = detail::delegate_factory<T, t>::create;

} // namespace delegates
} // namespace rome
