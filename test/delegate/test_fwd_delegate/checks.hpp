//
// Project: C++ delegates
//
// Copyright Roger Mettler 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <doctest.h>

#include <rome/delegate.hpp>
#include <utility>

#include "delegate_traits.hpp"

namespace test_rome_delegate {
namespace detail {
    namespace test_calling_delegate {
        template<typename Ret, typename Mock>
        struct checker {
            template<typename D, typename... Args>
            inline static bool call_with(const D& dgt, Args&&... args) {
                static_assert(!std::is_same<delegate_behavior<D>, rome::target_is_optional>::value,
                    "invalid template argument combination");

                ++Mock::expectedCalls.callOperator;
                const auto expectedResult = (*Mock::behavior)(std::forward<Args>(args)...);
                const auto actualResult   = dgt(std::forward<Args...>(args...));
                CHECK(expectedResult == actualResult);
                CHECK(Mock::expectedCalls == Mock::performedCalls);

                return (expectedResult == actualResult)
                       && (Mock::expectedCalls == Mock::performedCalls);
            }
        };

        template<typename Mock>
        struct checker<void, Mock> {
            template<typename D, typename... Args>
            inline static bool call_with(const D& dgt, Args&&... args) {
                ++Mock::expectedCalls.callOperator;
                dgt(std::forward<Args...>(args...));
                CHECK(Mock::expectedCalls == Mock::performedCalls);

                return (Mock::expectedCalls == Mock::performedCalls);
            }
        };
    }  // namespace test_calling_delegate

    namespace test_calling_empty_delegate {
        template<typename Behavior>
        struct checker;

        template<>
        struct checker<rome::target_is_mandatory> {
            template<typename D, typename... Args>
            inline static bool call_with(const D& dgt, Args&&... args) {
                try {
                    REQUIRE_THROWS_WITH_AS(static_cast<void>(dgt(std::forward<Args...>(args...))),
                        "rome::bad_delegate_call", rome::bad_delegate_call);
                    return true;
                }
                catch (...) {
                    return false;
                }
            }
        };

        template<>
        struct checker<rome::target_is_expected> {
            template<typename D, typename... Args>
            inline static bool call_with(const D& dgt, Args&&... args) {
                try {
                    REQUIRE_THROWS_WITH_AS(static_cast<void>(dgt(std::forward<Args...>(args...))),
                        "rome::bad_delegate_call", rome::bad_delegate_call);
                    return true;
                }
                catch (...) {
                    return false;
                }
            }
        };

        template<>
        struct checker<rome::target_is_optional> {
            template<typename D, typename... Args>
            inline static bool call_with(const D& dgt, Args&&... args) {
                try {
                    REQUIRE_NOTHROW(static_cast<void>(dgt(std::forward<Args...>(args...))));
                    return true;
                }
                catch (...) {
                    return false;
                }
            }
        };
    }  // namespace test_calling_empty_delegate

    namespace check_delegate_emptyness {
        template<typename T>
        bool checkEmpty(const T& dgt) {
            CHECK(dgt == nullptr);
            CHECK(nullptr == dgt);
            CHECK(!(dgt != nullptr));
            CHECK(!(nullptr != dgt));
            CHECK(!static_cast<bool>(dgt));
            CHECK(!dgt);
            return (dgt == nullptr) && (nullptr == dgt) && (!(dgt != nullptr))
                   && (!(nullptr != dgt)) && (!static_cast<bool>(dgt)) && (!dgt);
        }

        template<typename T>
        bool checkNotEmpty(const T& dgt) {
            CHECK(dgt != nullptr);
            CHECK(nullptr != dgt);
            CHECK(!(dgt == nullptr));
            CHECK(!(nullptr == dgt));
            CHECK(static_cast<bool>(dgt));
            CHECK(!!dgt);
            return (dgt != nullptr) && (nullptr != dgt) && (!(dgt == nullptr))
                   && (!(nullptr == dgt)) && (static_cast<bool>(dgt)) && (!!dgt);
        }
    }  // namespace check_delegate_emptyness

    template<typename D>
    class checker {
        const D& dgt_;

      public:
        checker(const D& dgt) : dgt_{dgt} {
        }

        template<typename... Args>
        bool callingEmptyDelegateWith(Args... args) {
            return detail::test_calling_empty_delegate::checker<delegate_behavior<D>>::call_with(
                dgt_, std::forward<Args>(args)...);
        }

        bool isEmpty() {
            return detail::check_delegate_emptyness::checkEmpty(dgt_);
        }

        bool isNotEmpty() {
            return detail::check_delegate_emptyness::checkNotEmpty(dgt_);
        }
    };

    template<typename Mock, typename D>
    class checker_with_mock {
        const D& dgt_;

      public:
        checker_with_mock(const D& dgt) : dgt_{dgt} {
        }

        template<typename... Args>
        bool callingItWith(Args... args) {
            return detail::test_calling_delegate::checker<delegate_return_type<D>, Mock>::call_with(
                dgt_, std::forward<Args>(args)...);
        }
    };
}  // namespace detail

template<typename D>
auto check(const D& dgt) {
    return detail::checker<D>(dgt);
}

template<typename Mock, typename D>
auto check(const D& dgt) {
    return detail::checker_with_mock<Mock, D>(dgt);
}
}  // namespace test_rome_delegate
