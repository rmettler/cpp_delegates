//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include "me/delegates/delegate.hpp"
#include <cassert>

namespace me {

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
    delegates::delegate<int(bool)>::create_from_non_static_member<A, &A::cefoo>(
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
    delegates::delegate<int(bool)>::create_from_non_static_member<A, &A::cefoo>(
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
    bool)>::create_from_non_static_const_member<A, &A::cecfoo>(&a);
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
    delegates::delegate<int(bool)>::create_from_function<&A::cesfoo>();
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
    delegates::delegate<int(bool)>::create_from_function<&cefoo>();
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
    // TODO: test dynamic usage of all the above delegates
    // TODO: extend test for setting and unsetting of all the delegates
    // (set one, set another or set one, unset, then set another)
}

} // namespace me

int main() { me::runTests(); }