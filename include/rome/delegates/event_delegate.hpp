//
// Project: C++ delegates
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

#pragma once

#include "delegate.hpp"
#include "detail/asserts.hpp"
#include "detail/event_delegate_argument_check.hpp"

namespace rome {
namespace delegates {

template <typename Signature>
class event_delegate
    : private detail::assert_invalid_event_delegate_signature<Signature> {
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
    using base_type::isSet;
    using base_type::operator bool;
    using base_type::operator!;
    using base_type::operator==;
    using base_type::operator!=;
    using base_type::operator();
    using base_type::create;
};

} // namespace delegates
} // namespace rome