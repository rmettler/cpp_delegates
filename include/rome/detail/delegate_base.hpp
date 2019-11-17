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
        void no_delete(buffer_type const&) {
        }


        template<typename Signature, typename EmptyInvoker>
        class delegate_base;

        template<typename Ret, typename... Args, typename EmptyInvoker>
        class delegate_base<Ret(Args...), EmptyInvoker> {
          private:
            // TODO: update types to be big enough for all used pointers!
            // buffer_ needs to be writable by 'operator()(Args...) const' if small buffer
            // optimization is used
            alignas(bufferAlignment()) mutable buffer_type buffer_ = nullptr;
            invoker_type<Ret, Args...> callee_                     = EmptyInvoker::invoke;
            deleter_type deleter_                                  = no_delete;

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
                delegate_base{std::move(orig)}.swap(*this);
                return *this;
            }
            delegate_base& operator=(std::nullptr_t) noexcept {
                delegate_base{}.swap(*this);
                return *this;
            }

            void swap(delegate_base& other) noexcept {
                using std::swap;
                swap(buffer_, other.buffer_);
                swap(callee_, other.callee_);
                swap(deleter_, other.deleter_);
            }

            constexpr operator bool() noexcept {
                return callee_ != static_cast<decltype(callee_)>(EmptyInvoker::invoke);
            }

            Ret operator()(Args... args) const {
                (*callee_)(buffer_, std::forward<Args>(args)...);
            }

            // Creates a new delegate_base and stores the passed non-static member function inside
            // its local buffer.
            template<Ret (*function)(Args...)>
            constexpr static delegate_base create() noexcept {
                delegate_base d;
                d.buffer_ = nullptr;
                d.callee_ = [](buffer_type const&, Args&&... args) -> Ret {
                    return (*function)(std::forward<Args>(args)...);
                };
                d.deleter_ = no_delete;
                return d;
            }

            // Creates a new delegate_base and stores the passed non-static member function inside
            // its local buffer.
            template<typename C, Ret (C::*method)(Args...)>
            static delegate_base create(C& obj) noexcept {
                delegate_base d;
                d.buffer_ = &obj;
                d.callee_ = [](buffer_type const& buffer, Args&&... args) -> Ret {
                    return (static_cast<C*>(buffer)->*method)(std::forward<Args>(args)...);
                };
                d.deleter_ = no_delete;
                return d;
            }

            // Creates a new delegate_base and stores the passed non-static const member function
            // inside its local buffer.
            template<typename C, Ret (C::*method)(Args...) const>
            static delegate_base create(const C& obj) noexcept {
                delegate_base d;
                d.buffer_ = const_cast<C*>(&obj);  // const will be added again before used
                d.callee_ = [](buffer_type const& buffer, Args&&... args) -> Ret {
                    return (static_cast<const C*>(buffer)->*method)(std::forward<Args>(args)...);
                };
                d.deleter_ = no_delete;
                return d;
            }

            // Creates a new delegate_base and stores the passed invokable inside its local
            // buffer.
            template<typename Invokable,
                std::enable_if_t<isSmallBufferOptimizable<Invokable>(), int> = 0>
            static delegate_base create(Invokable invokable) noexcept(
                std::is_nothrow_move_constructible<Invokable>::value) {
                // Reason for using const_cast here:
                // The functor needs be able to change its own members. Here, the functor and
                // its members are embedded in delegate_base::buffer_ using small buffer
                // optimization. Thus the functor needs to be able to change buffer_, even so
                // the delegate_base doesn't change its behavior.
                //! TODO: might lead to an error! when:
                //!   static const delegate d{delegate::create(bla)};
                //!   d(); // and this changes the delegate, which might be in RO memory!
                //! maybe set buffer_ mutable?
                //! maybe buffer_type const& can be replaced too with buffer_type&!
                delegate_base d;
                const auto ptr = ::new (&d.buffer_) Invokable(std::move(invokable));
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
        inline void swap(delegate_base<Ret(Args...), EmptyInvoker>& lhs,
            delegate_base<Ret(Args...), EmptyInvoker>& rhs) noexcept {
            lhs.swap(rhs);
        }

    }  // namespace delegate_base
}  // namespace detail
}  // namespace rome
