/*
 * Created: 2019-02-24
 * Author: Roger Mettler (roger@roger-mettler.ch)
 * -----
 * Last Modified: 2019-02-25 01:26:04
 * Modified By: Roger Mettler
 * -----
 * Copyright (c) 2019 Roger Mettler All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

namespace rome {
namespace delegates {
// TODO add a proper .clang-format!!!
// TODO check whether is function, member, static or non-static
//      std::is_function -> all functions
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
