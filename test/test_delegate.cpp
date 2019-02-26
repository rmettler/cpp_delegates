// 
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// 

#include "me/delegates/delegate.hpp"

namespace me {

struct Functor {
    bool operator()(int) { return false; }
};

struct A {
    bool foo(int) { return false; }
    bool cfoo(int) const { return false; }
    static bool sfoo(int) { return false; }
    Functor functor;
    bool (A::*pFoo)(int) = foo;
    bool (A::*pCFoo)(int) const = cfoo;
    bool (*pSFoo)(int) = sfoo;
    bool (&rSFoo)(int) = sfoo;
};

bool foo(int) { return false; }

A a;

void runTests()
{
    delegates::delegate<bool(int)> d =
        delegates::make_delegate<decltype(A::foo), A::foo>(&a);
}

} // namespace me

int main() { me::runTests(); }