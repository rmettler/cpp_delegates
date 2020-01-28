//
// Project: C++ delegates
//
// Copyright Roger Mettler 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

template<typename D>
struct delegate_traits;

template<template<typename, typename> class D, typename Ret, typename... Args, typename Behavior>
struct delegate_traits<D<Ret(Args...), Behavior>> {
    using behavior       = Behavior;
    using signature      = Ret(Args...);
    using argument_types = std::tuple<Args...>;
    using return_type    = Ret;
};

template<typename D>
using delegate_behavior = typename delegate_traits<D>::behavior;

template<typename D>
using delegate_signature = typename delegate_traits<D>::signature;

template<typename D>
using delegate_argument_types = typename delegate_traits<D>::argument_types;

template<typename D>
using delegate_return_type = typename delegate_traits<D>::return_type;
