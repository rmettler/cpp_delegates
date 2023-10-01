//
// Project: C++ delegates
//
// Copyright Roger Mettler 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//
// Provides the facilities to query the template parameters of a delegate.

#pragma once

#include <tuple>

namespace test {
namespace detail {
    template<typename D>
    struct delegate_traits;

    template<template<typename, typename> class D, typename Ret, typename... Args,
        typename Behavior>
    struct delegate_traits<D<Ret(Args...), Behavior>> {
        using behavior       = Behavior;
        using signature      = Ret(Args...);
        using argument_types = std::tuple<Args...>;
        using return_type    = Ret;
    };
}  // namespace detail

// Provides the set empty behavior 'Behavior' of given delegate 'D'.
template<typename D>
using delegate_behavior_t = typename detail::delegate_traits<D>::behavior;

// Provides the function signature 'Signature' of given delegate 'D'.
template<typename D>
using delegate_signature_t = typename detail::delegate_traits<D>::signature;

// Provides the Ith argument type in the function signature 'Signature' of given delegate 'D'.
template<std::size_t I, typename D>
using delegate_argument_type_t =
    std::tuple_element_t<I, typename detail::delegate_traits<D>::argument_types>;

// Provides the return type in the function signature 'Signature' of given delegate 'D'.
template<typename D>
using delegate_return_type_t = typename detail::delegate_traits<D>::return_type;
}  // namespace test