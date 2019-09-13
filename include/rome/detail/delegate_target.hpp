//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <algorithm>
#include <cassert>
#include <type_traits>

#include "rome/bad_delegate_call.hpp"

namespace rome {
namespace detail {

    template<typename Signature>
    class delegate_target;

    template<typename Ret, typename... Args>
    class delegate_target<Ret(Args...)> {
      private:
        using callee_type  = Ret (*)(void* const&, Args...);
        using deleter_type = void (*)(void* const&);

        static constexpr auto alignment() {
            return std::max(sizeof(void*), alignof(void*));
        }
        template<typename T>
        static constexpr bool isSmallBufferOptimizable() {
            return sizeof(T) <= sizeof(void*) && alignof(T) <= alignment();
        }

      public:
        alignas(alignment()) void* buffer_;
        callee_type callee_;
        deleter_type deleter_;

        constexpr delegate_target() noexcept                       = delete;
        constexpr delegate_target(const delegate_target&) noexcept = delete;
        constexpr delegate_target(delegate_target&& orig) noexcept = default;
        constexpr delegate_target(void* buffer, callee_type callee, deleter_type deleter) noexcept
            : buffer_{buffer}, callee_{callee}, deleter_{deleter} {
        }
        ~delegate_target() {
            (*deleter_)(buffer_);
        }
        constexpr delegate_target& operator=(const delegate_target&) noexcept = delete;
        constexpr delegate_target& operator=(delegate_target&& orig) noexcept = default;

        constexpr bool operator==(const delegate_target& rhs) const noexcept {
            return (buffer_ == rhs.buffer_) && (callee_ == rhs.callee_)
                   && (deleter_ == rhs.deleter_);
        }

        constexpr operator bool() {
            return false; // TODO
        }

        inline Ret operator()(Args... args) const {
            (*callee_)(buffer_, std::forward<Args>(args)...);
        }

        template<typename T, std::enable_if_t<isSmallBufferOptimizable<T>(), int> = 0>
        static delegate_target create(T t) {
            // Reason for using const_cast here:
            // The functor needs be able to change its own members. Here, the functor and
            // its members are embedded in delegate_target::buffer_ using small buffer
            // optimization. Thus the functor needs to be able to change buffer_, even so
            // the delegate_target doesn't change its behavior.
            auto callee = [](void* const& buffer, Args... args) -> Ret {
                auto pFunctor = const_cast<void*>(static_cast<void const*>(&buffer));
                return (*static_cast<T*>(pFunctor))(args...);
            };
            auto deleter = [](void* const& buffer) -> void {
                auto pFunctor = const_cast<void*>(static_cast<void const*>(&buffer));
                static_cast<T*>(pFunctor)->~T();
            };
            delegate_target target{nullptr, callee, deleter};
            auto ptr = ::new (&target.buffer_) T(std::move(t));
            assert(ptr == static_cast<T*>(static_cast<void*>(&target.buffer_)));
            return target;
        }

        template<typename T, std::enable_if_t<!isSmallBufferOptimizable<T>(), int> = 0>
        static delegate_target create(T t) {
            auto callee = [](void* const& buffer, Args... args) -> Ret {
                return (*static_cast<T*>(buffer))(args...);
            };
            auto deleter = [](void* const& buffer) -> void { delete static_cast<T*>(buffer); };
            return delegate_target{new T(std::move(t)), callee, deleter};
        }

        static void no_delete(void* const&) {
        }

        static Ret no_call(void* const&, Args...) {
        }

#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND))
        [[noreturn]] static Ret exception_call(void* const&, Args...) {
            throw rome::bad_delegate_call{};
        }
#else
        [[noreturn]] static Ret exception_call(void* const&, Args...) {
            std::terminate();
        }
#endif
    };

}  // namespace detail
}  // namespace rome
