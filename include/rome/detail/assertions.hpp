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
namespace detail {

    template<typename...>
    struct wrong : std::false_type {};

    struct ok {};

    namespace event_delegate {
        struct assert_signature {};

        template<typename Signature>
        struct assert_signature_ : assert_signature {
            constexpr assert_signature_() {
                static_assert(wrong<Signature>::value,
                    "Invalid template parameter. The first template parameter must be a function "
                    "signature of form 'void (Args...)' and all arguments 'Args...' of kind 'T', "
                    "'T&&', 'T const&' or 'T const*'. "
                    "E.g.: 'event_delgate<void (std::array<int,10> const&)>'");
            }
        };

        struct assert_valid_arguments {};

        template<typename... Args>
        struct assert_valid_arguments_ : assert_valid_arguments {
            constexpr assert_valid_arguments_() {
                static_assert(wrong<Args...>::value,
                    "Invalid argument in 'Args...' of 'event_delegate<void "
                    "(Args...)>'. All arguments must be of kind 'T', 'T&&', 'T const&' "
                    "or 'T const*'. "
                    "E.g.: 'event_delgate<void (std::array<int,10> const&)>'");
            }
        };
    };  // namespace event_delegate

}  // namespace detail
}  // namespace rome
