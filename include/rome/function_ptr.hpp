//
// Project: C++ delegates
// File content:
//   - function_ptr_t<Ret(Args...)>
//   - member_function_ptr<C, Ret(Args...)>
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <type_traits>

namespace rome {

template<typename Signature>
using function_ptr_t = std::add_pointer_t<Signature>;

namespace detail {
    template<typename C, typename Signature>
    struct member_function_ptr;

    template<typename C, typename Ret, typename... Args>
    struct member_function_ptr<C, Ret(Args...)> {
        using type = Ret (C::*)(Args...);
    };

    template<typename C, typename Ret, typename... Args>
    struct member_function_ptr<C, Ret(Args...) const> {
        using type = Ret (C::*)(Args...) const;
    };
}  // namespace detail

template<typename C, typename Signature>
using member_function_ptr_t = typename detail::member_function_ptr<C, Signature>::type;

}  // namespace rome
