//
// Project: delegates
// File content:
//   - event_delegate<void (Args...)>
//   - make_event_delegate<..>(..)
// TODO: file description
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

// TODO: for event_delegates: only allow values, const ref or move as arguments

#pragma once

#include "delegate.hpp"

namespace me {
namespace delegates {

template <typename T> class event_delegate;

template <typename... Args>
class event_delegate<void(Args...)> : delegate<void(Args...)> {
    // TODO add static assert to check the Args...
  private:
    using base_type = delegate<void(Args...)>;
  public:
    using base_type::delegate;
    using base_type::operator=;
    using base_type::operator();
    using base_type::isSet;
    using base_type::operator bool;
    using base_type::operator!;
    using base_type::create_from_function;
    using base_type::create_from_non_static_const_member;
    using base_type::create_from_non_static_member;
};

namespace detail {

template <typename T, T t> struct event_delegate_factory;

template <typename C, typename... Args, void (C::*pMem)(Args...)>
struct event_delegate_factory<void (C::*)(Args...), pMem> {
    // TODO add static assert to check the Args...
    constexpr static auto create(C *obj)
    {
        return event_delegate<void(
            Args...)>::template create_from_non_static_member<C, pMem>(obj);
    }
};

template <typename C, typename... Args, void (C::*pMem)(Args...) const>
struct event_delegate_factory<void (C::*)(Args...) const, pMem> {
    // TODO add static assert to check the Args...
    constexpr static auto create(C const *obj)
    {
        return event_delegate<void(Args...)>::
            template create_from_non_static_const_member<C, pMem>(obj);
    }
};

template <typename... Args, void (*pMem)(Args...)>
struct event_delegate_factory<void (*)(Args...), pMem> {
    // TODO add static assert to check the Args...
    constexpr static auto create()
    {
        return event_delegate<void(
            Args...)>::template create_from_function<pMem>();
    }
};
} // namespace detail

template <typename T, T t>
constexpr auto make_event_delegate =
    detail::event_delegate_factory<T, t>::create;

} // namespace delegates
} // namespace me