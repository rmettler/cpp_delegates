//
// Project: delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "asserts.hpp"
#include <type_traits>
#include <utility>

namespace me {
namespace delegates {
namespace detail {

template <typename T> constexpr bool is_value()
{
    return std::is_null_pointer<T>::value || std::is_integral<T>::value ||
           std::is_floating_point<T>::value || std::is_enum<T>::value ||
           std::is_union<T>::value || std::is_class<T>::value;
}

template <typename T> constexpr bool is_referenced_type_a_const_value()
{
    if (is_value<T>()) {
        return std::is_const<T>::value;
    }
    if (std::is_pointer<T>::value) {
        using TT = typename std::remove_pointer<T>::type;
        return is_referenced_type_a_const_value<TT>();
    }
    return false;
}

template <typename T> constexpr bool is_copied_moved_or_const_referenced_value()
{
    if (is_value<T>()) {
        return true;
    }
    if (std::is_rvalue_reference<T>::value) {
        using TT = typename std::remove_reference<T>::type;
        return is_copied_moved_or_const_referenced_value<TT>();
    }
    if (std::is_lvalue_reference<T>::value) {
        using TT = typename std::remove_reference<T>::type;
        return is_referenced_type_a_const_value<TT>();
    }
    return is_referenced_type_a_const_value<T>();
}

template <typename... Args>
using check_event_delegate_arguments = std::conditional_t<
    std::is_same<
        std::integer_sequence<
            bool, true,
            is_copied_moved_or_const_referenced_value<Args>()...>,
        std::integer_sequence<
            bool, is_copied_moved_or_const_referenced_value<Args>()...,
            true>>::value,
    ok, invalid_event_delegate_argument<Args...>>;

} // namespace detail
} // namespace delegates
} // namespace me
