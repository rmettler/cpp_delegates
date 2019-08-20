//
// Project: C++ delegates
// File content:
//   - bad_delegate_call
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <exception>

namespace rome {

class bad_delegate_call : public std::exception {
  public:
    const char* what() const noexcept override {
        return "rome::bad_delegate_call";
    }
};

}  // namespace rome
