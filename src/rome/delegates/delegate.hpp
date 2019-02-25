// 
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// 


namespace rome {
namespace delegates {
// TODO add a proper .clang-format!!!
// TODO check whether is function, member, static or non-static
//      std::is_function -> all functions
//      std::is_member_pointer, is_member_object_pointer, is_member_function_pointer
template <typename T> struct delegate;

template <typename Ret, typename... Args> struct delegate<Ret(Args...)> {
    // TODO enhance by passing the stub as template argument (or defaulting to a dynamic one?)
    /* data */
};

template <typename T, T t> struct delegate_creator;

template <typename Ret, typename... Args, typename C, Ret (C::*pMem)(Args...)>
struct delegate_creator<Ret (C::*)(Args...), pMem> {
    static constexpr auto create(C *)
    {
        // TODO create the specific type
        return delegate<Ret(Args...)>{};
    }
};

template <typename T, T t>
constexpr auto make_delegate = delegate_creator<T, t>::create;

} // namespace delegates
} // namespace rome
