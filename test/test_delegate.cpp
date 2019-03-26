//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include "rome/delegates.hpp"
#include <cassert>
#include <iostream>

namespace rome {

struct Functor {
    int operator()(bool) { return 1; }
};

struct A {
    int foo(bool) { return 2; }
    constexpr int cefoo(bool) { return 12; }
    int cfoo(bool) const { return 3; }
    constexpr int cecfoo(bool) const { return 13; }
    static int sfoo(bool) { return 4; }
    constexpr static int cesfoo(bool) { return 14; }
    Functor functor;
    int (A::*pFoo)(bool) = foo;
    int (A::*pCFoo)(bool) const = cfoo;
    int (*pSFoo)(bool) = sfoo;
    int (&rSFoo)(bool) = sfoo;
};

int foo(bool) { return 5; }
constexpr int cefoo(bool) { return 15; }

static constexpr A a;

#if 0
namespace test_not_set {
static constexpr delegates::delegate<int(bool)> implcitlyNotSet{};
static_assert(false == implcitlyNotSet.isSet(), "");
static_assert(false == implcitlyNotSet, "");
static_assert(true == !implcitlyNotSet, "");

static constexpr delegates::delegate<int(bool)> explicitlyNotSet{nullptr};
static_assert(false == explicitlyNotSet.isSet(), "");
static_assert(false == explicitlyNotSet, "");
static_assert(true == !explicitlyNotSet, "");
} // namespace test_not_set

namespace test_constexpr_method {
static constexpr auto direct =
    delegates::delegate<int(bool)>::createFromNonStaticMemberFunction<A, &A::cefoo>(
        const_cast<A *>(&a));
static_assert(12 == direct(true), "");
static_assert(true == direct.isSet(), "");
static_assert(true == direct, "");
static_assert(false == !direct, "");

static constexpr auto factory =
    delegates::make_delegate<decltype(&A::cefoo), &A::cefoo>(
        const_cast<A *>(&a));
static_assert(12 == factory(true), "");
static_assert(true == factory.isSet(), "");
static_assert(true == factory, "");
static_assert(false == !factory, "");

static constexpr auto directNullObj =
    delegates::delegate<int(bool)>::createFromNonStaticMemberFunction<A, &A::cefoo>(
        nullptr);
static_assert(false == directNullObj.isSet(), "");
static_assert(false == directNullObj, "");
static_assert(true == !directNullObj, "");

static constexpr auto factoryNullObj =
    delegates::make_delegate<decltype(&A::cefoo), &A::cefoo>(nullptr);
static_assert(false == factoryNullObj.isSet(), "");
static_assert(false == factoryNullObj, "");
static_assert(true == !factoryNullObj, "");
} // namespace test_constexpr_method

namespace test_constexpr_const_method {
static constexpr auto direct = delegates::delegate<int(
    bool)>::createFromNonStaticConstMemberFunction<A, &A::cecfoo>(&a);
static_assert(13 == direct(true), "");
static_assert(true == direct.isSet(), "");
static_assert(true == direct, "");
static_assert(false == !direct, "");

static constexpr auto factory =
    delegates::make_delegate<decltype(&A::cecfoo), &A::cecfoo>(&a);
static_assert(13 == factory(true), "");
static_assert(true == factory.isSet(), "");
static_assert(true == factory, "");
static_assert(false == !factory, "");

// Passing a nullptr as object lead to a non constexpr call and throw a compile
// error (GCC 8.2). Hence these tests are omitted.
} // namespace test_constexpr_const_method

namespace test_constexpr_static_method {
static constexpr auto direct =
    delegates::delegate<int(bool)>::createFromFunction<&A::cesfoo>();
static_assert(14 == direct(true), "");
static_assert(true == direct.isSet(), "");
static_assert(true == direct, "");
static_assert(false == !direct, "");

static constexpr auto factory =
    delegates::make_delegate<decltype(&A::cesfoo), &A::cesfoo>();
static_assert(14 == factory(true), "");
static_assert(true == factory.isSet(), "");
static_assert(true == factory, "");
static_assert(false == !factory, "");
} // namespace test_constexpr_static_method

namespace test_constexpr_function {
static constexpr auto direct =
    delegates::delegate<int(bool)>::createFromFunction<&cefoo>();
static_assert(15 == direct(true), "");
static_assert(true == direct.isSet(), "");
static_assert(true == direct, "");
static_assert(false == !direct, "");

static constexpr auto factory =
    delegates::make_delegate<decltype(&cefoo), &cefoo>();
static_assert(15 == factory(true), "");
static_assert(true == factory.isSet(), "");
static_assert(true == factory, "");
static_assert(false == !factory, "");
} // namespace test_constexpr_function

void fooo(int i = 1) { assert(i == 11); }
struct B {
    void foo(int) { }
};

// TODO: test class members which are callable but no functions (functor,
// references of functions)
// TODO: test namespace variables which are callable but no functions (functor,
// references of functions)
// TODO: test with fancy arguments (modifiers, lvalue and rvalue references, non
// constexpr arguments)
// TODO: test with fancy return types (modifiers, lvalue and rvalue references,
// non constexpr types)
// TODO: test with void return and no or multiple arguments
// TODO: test usage of lambdas

void runTests()
{
    auto d = delegates::make_delegate<decltype(&fooo), &fooo>();
    d(11);
    auto ed = delegates::make_event_delegate<decltype(&fooo), &fooo>();
    ed(11);
    auto ed2 = delegates::event_delegate<void(int)>{};
    B b;
    ed2 = delegates::make_event_delegate<decltype(&B::foo), &B::foo>(&b);
    ed2 = delegates::event_delegate<void(
        int)>::createFromNonStaticMemberFunction<B, &B::foo>(&b);
    using Bla = delegates::event_delegate<void(int&, int*)>;
    Bla bla = {};
    // TODO: test dynamic usage of all the above delegates
    // TODO: extend test for setting and unsetting of all the delegates
    // (set one, set another or set one, unset, then set another)
}
#endif

constexpr auto create()
{
    delegates::delegate<int(bool)> indirect;
    indirect =
        delegates::delegate<int(bool)>::create<A, &A::cfoo>(const_cast<A &>(a));
    return indirect;
}

void fooo(const int &) {}

void runTests()
{
    // TODO: constexpr für alle member funktionen testen ausser operator()
    //       operator() nur für constexpr function testen
    constexpr auto d = create();
    assert(3 == d(true));
    static_assert(true == d.isSet(), "");
    static_assert(true == d, "");
    static_assert(false == !d, "");

    constexpr auto dd = delegates::delegate<int(bool)>::create<&cefoo>();
    constexpr int ret = dd(true);

    auto ddd = delegates::make_event_delegate<decltype(&fooo), &fooo>();
    ddd = nullptr;
    int x;
    ddd(x);
}

} // namespace rome

int main()
{
    std::cout << "Running tests" << std::endl;
    rome::runTests();
    std::cout << "Tests finished" << std::endl;
}