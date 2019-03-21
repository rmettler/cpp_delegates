//
// Project: delegates
// File content:
//   - make_event_delegate<..>(..)
// See the documentation for more information.
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "event_delegate.hpp"

namespace me {
namespace delegates {

namespace detail {

template <typename T, T t> struct event_delegate_factory;

template <typename C, typename... Args, void (C::*pMem)(Args...)>
struct event_delegate_factory<void (C::*)(Args...), pMem> {
    // TODO add static assert to check the Args...
    constexpr static auto create(C *obj)
    {
        return event_delegate<void(
            Args...)>::template createFromNonStaticMemberFunction<C, pMem>(obj);
    }
};

template <typename C, typename... Args, void (C::*pMem)(Args...) const>
struct event_delegate_factory<void (C::*)(Args...) const, pMem> {
    // TODO add static assert to check the Args...
    constexpr static auto create(C const *obj)
    {
        return event_delegate<void(Args...)>::
            template createFromNonStaticConstMemberFunction<C, pMem>(obj);
    }
};

template <typename... Args, void (*pMem)(Args...)>
struct event_delegate_factory<void (*)(Args...), pMem> {
    // TODO add static assert to check the Args...
    constexpr static auto create()
    {
        return event_delegate<void(
            Args...)>::template createFromFunction<pMem>();
    }
};
} // namespace detail

template <typename T, T t>
constexpr auto make_event_delegate =
    detail::event_delegate_factory<T, t>::create;

} // namespace delegates
} // namespace me