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
    namespace delegate_base {

        using buffer_type = void*;
        template<typename Ret, typename... Args>
        using invoker_type = Ret (*)(buffer_type const&, Args&&...);
        using deleter_type = void (*)(buffer_type const&);

        static constexpr auto bufferAlignment() {
            return std::max(sizeof(buffer_type), alignof(buffer_type));
        }

        // Whether the type T can be locally stored in buffer_type or not.
        template<typename T>
        static constexpr bool isSmallBufferOptimizable() {
            return (sizeof(T) <= sizeof(buffer_type)) && (alignof(T) <= bufferAlignment());
        }

        // Used as template parameter for delegate_base. Calls the provided invoke function when
        // empty.
        struct no_call_invoker {
            template<typename Ret, typename... Args,
                typename = std::enable_if_t<std::is_same<Ret, void>::value>>
            static Ret invoke(buffer_type const&, Args&&...) {
            }
        };
        // Used as template parameter for delegate_base. Calls the provided invoke function when
        // empty.
        struct exception_call_invoker {
            template<typename Ret, typename... Args>
            [[noreturn]] static Ret invoke(buffer_type const&, Args&&...) {
#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND))
                throw rome::bad_delegate_call{};
#else
                std::terminate();
#endif
            }
        };

        // Used as deleter in delegate_base when no destruction of buffer is needed.
        void no_delete(buffer_type const& buffer) {
        }


        template<typename Signature, typename EmptyInvoker>
        class delegate_base;

        template<typename Ret, typename... Args, typename EmptyInvoker>
        class delegate_base<Ret(Args...), EmptyInvoker> {
          private:
            alignas(bufferAlignment()) buffer_type buffer_ = nullptr;
            invoker_type<Ret, Args...> callee_             = EmptyInvoker::invoke;
            deleter_type deleter_                          = no_delete;

          public:
            constexpr delegate_base() noexcept           = default;
            delegate_base(delegate_base const&) noexcept = delete;
            delegate_base(delegate_base&& orig) noexcept {
                orig.swap(*this);
            }

            ~delegate_base() {
                (*deleter_)(buffer_);
            }

            delegate_base& operator=(delegate_base const&) noexcept = delete;
            delegate_base& operator=(delegate_base&& orig) noexcept {
                delegate_base(std::move(orig)).swap(*this);
            };
            delegate_base& operator=(std::nullptr_t) noexcept {
                *this = delegate_base();
            }

            void swap(delegate_base& other) noexcept {
                using std::swap;
                swap(buffer_, other.buffer_);
                swap(callee_, other.callee_);
                swap(deleter_, other.deleter_);
            }

            constexpr bool operator==(delegate_base const& rhs) const noexcept {
                return (buffer_ == rhs.buffer_) && (callee_ == rhs.callee_)
                       && (deleter_ == rhs.deleter_);
            }

            constexpr operator bool() noexcept {
                return callee_ != static_cast<decltype(callee_)>(EmptyInvoker::invoke);
            }

            // TODO: use perfect forwarding here!
            Ret operator()(Args... args) const {
                (*callee_)(buffer_, std::forward<Args>(args)...);
            }

            // Creates a new delegate_base and stores the passed non-static member function inside
            // its local buffer.
            template<Ret (*function)(Args...)>
            static delegate_base create() {
                delegate_base d;
                d.buffer_ = nullptr;
                d.callee_ = [](buffer_type const& buffer, Args&&... args) -> Ret {
                    return (*function)(std::forward<Args>(args)...);
                };
                d.deleter_ = no_delete;
                return d;
            }

            // Creates a new delegate_base and stores the passed non-static member function inside
            // its local buffer.
            template<typename C, Ret (C::*method)(Args...)>
            static delegate_base create(C& obj) {
                delegate_base d;
                d.buffer_ = &obj;
                d.callee_ = [](buffer_type const& buffer, Args&&... args) -> Ret {
                    return (static_cast<C*>(buffer)->*method)(std::forward<Args>(args)...);
                };
                d.deleter_ = no_delete;
                return d;
            }

            // Creates a new delegate_base and stores the passed invokable inside its local
            // buffer.
            template<typename Invokable,
                std::enable_if_t<isSmallBufferOptimizable<Invokable>(), int> = 0>
            static delegate_base create(Invokable invokable) {
                // Reason for using const_cast here:
                // The functor needs be able to change its own members. Here, the functor and
                // its members are embedded in delegate_base::buffer_ using small buffer
                // optimization. Thus the functor needs to be able to change buffer_, even so
                // the delegate_base doesn't change its behavior.
                delegate_base d;
                auto ptr = ::new (&d.buffer_) Invokable(std::move(invokable));
                assert(ptr == static_cast<Invokable*>(static_cast<void*>(&d.buffer_)));
                d.callee_ = [](buffer_type const& buffer, Args&&... args) -> Ret {
                    auto pFunctor = static_cast<void*>(const_cast<buffer_type*>(&buffer));
                    return (*static_cast<Invokable*>(pFunctor))(std::forward<Args>(args)...);
                };
                d.deleter_ = [](buffer_type const& buffer) -> void {
                    auto pFunctor = static_cast<void*>(const_cast<buffer_type*>(&buffer));
                    static_cast<Invokable*>(pFunctor)->~Invokable();
                };
                return d;
            }

            // Creates a new delegate_base and stores the passed invokable at a new location
            // outside its local buffer.
            template<typename Invokable,
                std::enable_if_t<!isSmallBufferOptimizable<Invokable>(), int> = 0>
            static delegate_base create(Invokable invokable) {
                delegate_base d;
                d.buffer_ = new Invokable(std::move(invokable));
                d.callee_ = [](buffer_type const& buffer, Args&&... args) -> Ret {
                    return (*static_cast<Invokable*>(buffer))(std::forward<Args>(args)...);
                };
                d.deleter_ = [](buffer_type const& buffer) -> void {
                    delete static_cast<Invokable*>(buffer);
                };
                return d;
            }
        };

        template<typename Ret, typename... Args, typename EmptyInvoker>
        inline void swap(delegate_base<Ret(Args...), EmptyInvoker>& lhs, delegate_base<Ret(Args...), EmptyInvoker>& rhs) noexcept {
            lhs.swap(rhs);
        }

    }  // namespace delegate_base
}  // namespace detail
}  // namespace rome
