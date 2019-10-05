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
    namespace delegate_impl {

        using buffer_type = void*;
        template<typename Ret, typename... Args>
        using invoker_type = Ret (*)(buffer_type const&, Args&&...);
        using deleter_type = void (*)(buffer_type const&);

        static constexpr auto bufferAlignment() {
            return std::max(sizeof(buffer_type), alignof(buffer_type));
        }

        // Whether the T can be locally stored in buffer_type or not.
        template<typename T>
        static constexpr bool isSmallBufferOptimizable() {
            return (sizeof(T) <= sizeof(buffer_type)) && (alignof(T) <= bufferAlignment());
        }

        // Used as template parameter for delegate_impl to set invoker to when empty.
        struct no_call_invoker {
            template<typename Ret, typename... Args,
                typename = std::enable_if_t<std::is_same<Ret, void>::value>>
            static Ret invoke(buffer_type const&, Args&&...) {
            }
        };
        // Used as template parameter for delegate_impl to set invoker to when empty.
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

        // Used as deleter in delegate_impl when no destruction of buffer is needed.
        void no_delete(buffer_type const& buffer) {
        }


        template<typename Signature, typename EmptyInvoker>
        class delegate_impl;

        template<typename Ret, typename... Args, typename EmptyInvoker>
        class delegate_impl<Ret(Args...), EmptyInvoker> {
          private:
            alignas(bufferAlignment()) buffer_type buffer_ = nullptr;
            invoker_type<Ret, Args...> callee_             = EmptyInvoker::invoke;
            deleter_type deleter_                          = no_delete;

          public:
            constexpr delegate_impl() noexcept           = default;
            delegate_impl(delegate_impl const&) noexcept = delete;
            delegate_impl(delegate_impl&& orig) noexcept {
                orig.swap(*this);
            }

            ~delegate_impl() {
                (*deleter_)(buffer_);
            }

            delegate_impl& operator=(delegate_impl const&) noexcept = delete;
            delegate_impl& operator=(delegate_impl&& orig) noexcept {
                delegate_impl(std::move(orig)).swap(*this);
            };
            delegate_impl& operator=(std::nullptr_t) {
                *this = delegate_impl();
            }

            void swap(delegate_impl& other) {
                std::swap(buffer_, other.buffer_);
                std::swap(callee_, other.callee_);
                std::swap(deleter_, other.deleter_);
            }

            constexpr bool operator==(delegate_impl const& rhs) const noexcept {
                return (buffer_ == rhs.buffer_) && (callee_ == rhs.callee_)
                       && (deleter_ == rhs.deleter_);
            }

            constexpr operator bool() {
                return callee_ != static_cast<decltype(callee_)>(EmptyInvoker::invoke);
            }

            // TODO: use perfect forwarding here!
            inline Ret operator()(Args... args) const {
                (*callee_)(buffer_, std::forward<Args>(args)...);
            }

            // Creates a new delegate_impl and stores the passed static function inside its local
            // buffer.
            // TODO: compare this solution with the lower one and then romove it!
            // TODO: prevent misuse with similar applications of this function (T t)
            template<std::enable_if_t<isSmallBufferOptimizable<Ret (*)(Args...)>(), int> = 0>
            static delegate_impl create(Ret (*function)(Args...)) {
                // Reason for using const_cast here:
                // The functor needs be able to change its own members. Here, the functor and
                // its members are embedded in delegate_impl::buffer_ using small buffer
                // optimization. Thus the functor needs to be able to change buffer_, even so
                // the delegate_impl doesn't change its behavior.
                using Function = Ret (*)(Args...);
                delegate_impl d;
                auto ptr = ::new (&d.buffer_) Function(std::move(function));
                assert(ptr == static_cast<Function*>(static_cast<void*>(&d.buffer_)));
                d.callee_ = [](buffer_type const& buffer, Args&&... args) -> Ret {
                    auto pFunctor = static_cast<void*>(const_cast<buffer_type*>(&buffer));
                    return (*static_cast<Function*>(pFunctor))(std::forward<Args>(args)...);
                };
                d.deleter_ = [](buffer_type const& buffer) -> void {
                    auto pFunctor = static_cast<void*>(const_cast<buffer_type*>(&buffer));
                    static_cast<Function*>(pFunctor)->~Function();
                };
                return d;
            }

            // Creates a new delegate_impl and stores the passed non-static member function inside
            // its local buffer.
            template<Ret (*function)(Args...)>
            static delegate_impl create() {
                delegate_impl d;
                d.buffer_ = nullptr;
                d.callee_ = [](buffer_type const& buffer, Args&&... args) -> Ret {
                    return (*function)(std::forward<Args>(args)...);
                };
                d.deleter_ = no_delete;
                return d;
            }

            // Creates a new delegate_impl and stores the passed non-static member function inside
            // its local buffer.
            template<typename C, Ret (C::*method)(Args...)>
            static delegate_impl create(C& obj) {
                delegate_impl d;
                d.buffer_ = &obj;
                d.callee_ = [](buffer_type const& buffer, Args&&... args) -> Ret {
                    return (static_cast<C*>(buffer)->*method)(std::forward<Args>(args)...);
                };
                d.deleter_ = no_delete;
                return d;
            }

            // Creates a new delegate_impl and stores the passed invokable inside its local
            // buffer.
            template<typename Invokable,
                std::enable_if_t<isSmallBufferOptimizable<Invokable>(), int> = 0>
            static delegate_impl create(Invokable invokable) {
                // Reason for using const_cast here:
                // The functor needs be able to change its own members. Here, the functor and
                // its members are embedded in delegate_impl::buffer_ using small buffer
                // optimization. Thus the functor needs to be able to change buffer_, even so
                // the delegate_impl doesn't change its behavior.
                delegate_impl d;
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

            // Creates a new delegate_impl and stores the passed invokable at a new location
            // outside its local buffer.
            template<typename Invokable,
                std::enable_if_t<!isSmallBufferOptimizable<Invokable>(), int> = 0>
            static delegate_impl create(Invokable invokable) {
                delegate_impl d;
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

    }  // namespace delegate_impl
}  // namespace detail
}  // namespace rome
