//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <type_traits>

namespace rome {
namespace delegates {
namespace detail {

template <typename...> struct wrong : std::false_type {
};

struct ok {
};

struct invalid_signature {
};

template <typename Signature>
struct invalid_delegate_signature : invalid_signature {
    constexpr invalid_delegate_signature()
    {
        static_assert(
            wrong<Signature>::value,
            "Invalid template parameters. There must be exactly one template "
            "parameter with a function signature of the form 'Ret (Args...)'."
            "E.g.: 'delgate<float (std::array<int,10>&)>'");
    }
};

template <typename Signature>
struct invalid_event_delegate_signature : invalid_signature {
    constexpr invalid_event_delegate_signature()
    {
        static_assert(
            wrong<Signature>::value,
            "Invalid template parameters. There must be exactly one template "
            "parameter with a function signature of the form 'void (Args...)' "
            "and all arguments 'Args...' of kind 'T', 'T&&', 'T const&' or 'T "
            "const*'. "
            "E.g.: 'event_delgate<void (std::array<int,10> const&)>'");
    }
};

struct invalid_argument {
};

template <typename... Args>
struct invalid_event_delegate_argument : invalid_argument {
    constexpr invalid_event_delegate_argument()
    {
        static_assert(
            wrong<Args...>::value,
            "Invalid argument in 'Args...' of 'event_delegate<void "
            "(Args...)>'. All arguments must be of kind 'T', 'T&&', 'T const&' "
            "or 'T const*'. "
            "E.g.: 'event_delgate<void (std::array<int,10> const&)>'");
    }
};

} // namespace detail
} // namespace delegates
} // namespace rome
