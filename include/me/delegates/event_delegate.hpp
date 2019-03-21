//
// Project: delegates
// File content:
//   - event_delegate<void (Args...)>
//   - make_event_delegate<..>(..)
// See the documentation for more information.
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

// TODO: for event_delegates: only allow values, const ref or move as arguments
// std::is_lvalue_reference<>::value && the referenced value is const
// std::is_pointer && the pointed value is const

#pragma once

#include "delegate.hpp"
#include "detail/asserts.hpp"
#include "detail/event_delegate_argument_check.hpp"

namespace me {
namespace delegates {

template <typename Signature>
class event_delegate : private detail::invalid_event_delegate_signature<Signature> {
};

template <typename... Args>
class event_delegate<void(Args...)>
    : private delegate<void(Args...)>,
      private detail::check_event_delegate_arguments<Args...> {
  private:
    using base_type = delegate<void(Args...)>;

  public:
    using base_type::delegate;
    using base_type::operator=;
    using base_type::operator();
    using base_type::isSet;
    using base_type::operator bool;
    using base_type::operator!;
    using base_type::createFromFunction;
    using base_type::createFromNonStaticConstMemberFunction;
    using base_type::createFromNonStaticMemberFunction;
};

} // namespace delegates
} // namespace me