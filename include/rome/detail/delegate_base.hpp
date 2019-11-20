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
        // TODO: instead of passing the Invoker by class, invoke the ptr as 'initialTarget'
        // TODO: inline these types into delegate_base and remove delegate_base namespace
        using buffer_type = void*;
        template<typename Ret, typename... Args>
        using invoker_type = Ret (*)(buffer_type&, Args&&...);
        using deleter_type = void (*)(buffer_type&);

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
            template<typename Ret, typename... Args>
            static inline Ret invoke(buffer_type&, Args&&...) {
            }
        };

        // Used as template parameter for delegate_base. Calls the provided invoke function when
        // empty.
        struct exception_call_invoker {
            template<typename Ret, typename... Args>
            [[noreturn]] static inline Ret invoke(buffer_type&, Args&&...) {
#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND))
                throw rome::bad_delegate_call{};
#else
                std::terminate();
#endif
            }
        };

        // Used as template parameter for delegate_base. Sets nullptr as the function pointer. Is
        // only used for invalid template parameter combinations.
        struct nullptr_invoker {
            static constexpr auto invoke = nullptr;
        };

        // Used as deleter in delegate_base when no destruction of buffer is needed.
        inline void no_delete(buffer_type&) {
        }

        template<typename Signature, typename EmptyInvoker>
        class delegate_base;

        template<typename Ret, typename... Args, typename EmptyInvoker>
        class delegate_base<Ret(Args...), EmptyInvoker> {
          private:
            // buffer_ needs to be writable by 'operator()(Args...) const' if small buffer
            // optimization is used
            alignas(bufferAlignment()) mutable buffer_type buffer_ = nullptr;
            invoker_type<Ret, Args...> callee_                     = EmptyInvoker::invoke;
            deleter_type deleter_                                  = no_delete;

          public:
            constexpr delegate_base() noexcept           = default;
            delegate_base(const delegate_base&) noexcept = delete;
            delegate_base(delegate_base&& orig) noexcept {
                orig.swap(*this);
            }

            ~delegate_base() {
                (*deleter_)(buffer_);
            }

            delegate_base& operator=(const delegate_base&) noexcept = delete;
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

            constexpr explicit operator bool() const noexcept {
                return callee_ != static_cast<decltype(callee_)>(EmptyInvoker::invoke);
            }

            Ret operator()(Args... args) const {
                return (*callee_)(buffer_, std::forward<Args>(args)...);
            }

            // Creates a new delegate_base and stores the passed function or static member function
            // inside its local buffer.
            template<Ret (*function)(Args...)>
            constexpr static delegate_base create() noexcept {
                delegate_base d;
                d.buffer_ = nullptr;
                d.callee_ = [](buffer_type&, Args&&... args) -> Ret {
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
                d.callee_ = [](buffer_type& buffer, Args&&... args) -> Ret {
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
                d.callee_ = [](buffer_type& buffer, Args&&... args) -> Ret {
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
                delegate_base d;
                const auto ptr = ::new (&d.buffer_) Invokable(std::move(invokable));
                d.callee_ = [](buffer_type& buffer, Args&&... args) -> Ret {
                    auto pFunctor = static_cast<void*>(&buffer);
                    return static_cast<Invokable*>(pFunctor)->operator()(
                        std::forward<Args>(args)...);
                };
                d.deleter_ = [](buffer_type& buffer) -> void {
                    auto pFunctor = static_cast<void*>(&buffer);
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
                d.callee_ = [](buffer_type& buffer, Args&&... args) -> Ret {
                    return static_cast<Invokable*>(buffer)->operator()(std::forward<Args>(args)...);
                };
                d.deleter_ = [](buffer_type& buffer) -> void {
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
