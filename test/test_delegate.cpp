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
#include <doctest.h>
#include <tuple>
#include <type_traits>

#if 0
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

static constexpr auto factory =make
    delegates::_delegate<decltype(&A::cesfoo), &A::cesfoo>();
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

#endif

struct TestVal {
    int val = 0;
    void reset() { val = 0; }
    void count(int count, bool plus)
    {
        if (plus) {
            val += count;
        }
        else {
            val -= count;
        }
    }
};

struct Functor : TestVal {
    void operator()(int count, const bool &plus)
    {
        TestVal::count(count, plus);
    }
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

TestVal testVal;

void foo(int count, const bool &plus) { testVal.count(count, plus); }
constexpr int cefoo(bool) { return 15; }

static constexpr A a;

template class ::rome::delegates::event_delegate<void(int, const bool &)>;

template class ::rome::delegates::delegate<void(void)>;
template class ::rome::delegates::delegate<void(int)>;
template class ::rome::delegates::delegate<int(void)>;
template class ::rome::delegates::delegate<int(int)>;
struct C;
template class ::rome::delegates::delegate<void(C &)>;
template class ::rome::delegates::delegate<C &(C &)>;
template class ::rome::delegates::delegate<void(const C &)>;
template class ::rome::delegates::delegate<const C &(const C &)>;
template class ::rome::delegates::delegate<void(volatile C &)>;
template class ::rome::delegates::delegate<volatile C &(volatile C &)>;
template class ::rome::delegates::delegate<void(C &&)>;
template class ::rome::delegates::delegate<C &&(C &&)>;
template class ::rome::delegates::delegate<void(C *)>;
template class ::rome::delegates::delegate<C *(C *)>;
template class ::rome::delegates::delegate<void(const C *)>;
template class ::rome::delegates::delegate<const C *(const C *)>;
// TODO: continue

TEST_SUITE("delegate")
{
    // helper to check whether the delegate with given signature has a base
    // class which would static_assert on creation
    template <typename TDelegate>
    constexpr auto asserts_signature =
        std::is_base_of<rome::delegates::detail::assert_invalid_signature,
                        TDelegate>::value;

    // helpers to create a whole lot of function argument combinations of
    // values, pointers, references and cv-modifiers
    template <typename... T>
    using add_ref = rome::delegates::delegate<int(T..., T & ..., T && ...)>;

    template <typename... T>
    using add_cv =
        add_ref<T..., T const..., T volatile..., T const volatile...>;

    template <typename... T>
    using add_ptr = add_cv<T..., T *..., T const *..., T volatile *...,
                           T const volatile *...>;

    template <typename... T>
    using add_ptr_ptr = add_ptr<T..., T *..., T const *..., T volatile *...,
                                T const volatile *...>;

    template <typename... T>
    using add_ptr_ptr_ptr = add_ptr_ptr<T..., T *..., T const *...,
                                        T volatile *..., T const volatile *...>;

    TEST_CASE("When a delegate is declared, exactly one template argument of "
              "type 'Ret(Args...)' must be given.")
    {
        using rome::delegates::delegate;

        class A {
        };
        CHECK(asserts_signature<delegate<void>>);
        CHECK(asserts_signature<delegate<int(*)[10]>>);
        CHECK(asserts_signature<delegate<int(&)[10]>>);
        CHECK(asserts_signature<delegate<void (*)()>>);
        CHECK(asserts_signature<delegate<void (&)()>>);
        CHECK(asserts_signature<delegate<void (A::*)()>>);
        CHECK(!asserts_signature<delegate<void()>>);
        CHECK(!asserts_signature<delegate<void(void)>>);
        CHECK(!asserts_signature<delegate<void(std::nullptr_t)>>);
        CHECK(!asserts_signature<delegate<void(int)>>);
        CHECK(!asserts_signature<delegate<int(void)>>);
        CHECK(!asserts_signature<delegate<int(int)>>);
        CHECK(!asserts_signature<delegate<int(int, int, int, int, int)>>);
    }

    // TODO: add full tests for all of the following, once for args, once for
    // return:
    // void
    // std::nullptr_t
    // int, bool, char, usigned int, double
    // int&,
    // int&&,
    // int*
    // class
    // enum
    // enum class
    // union
    // pointer to array, reference to array: int (*)[10], int (&)[10]
    // pointer to function, reference to function: int (*)(bool), int (&)(bool)
    // pointer to member object: int A::*
    // pointer to member function: int (A::*)(bool)

    TEST_CASE("The arguments of the delegate signature can contain any "
              "possible types also allowed for functions.")
    {
        using rome::delegates::delegate;

        SUBCASE(
            "Any combination of values, pointers, lvalue references, rvalue "
            "references or const and volatile modifiers also allowed for "
            "functions are allowed as arguments for the delegate signature.")
        {
            class A {
            };
            add_ptr_ptr_ptr<A> many_args_delegate{};
            CHECK(many_args_delegate.isSet() == false);
            CHECK(many_args_delegate == false);
            CHECK(!many_args_delegate == true);
        }
        // TODO: SUBCASE("TODO: fully test void(int)") {}
        // TODO: SUBCASE("TODO: fully test int(void)") {}
        // TODO: SUBCASE("TODO: add full test for any other argument type kind
        // (see above)") {}
        // TODO: SUBCASE("TODO: add full test for any other return type kind
        // (see above)") {}
    }

    TEST_CASE("Testing delegate<void(int)>")
    {
        using rome::delegates::delegate;

        delegate<void(int)> d;
        CHECK(d.isSet() == false);
        CHECK(d == false);
        CHECK(!d == true);
    }
    // TODO: add test case for make_delegate
} // TEST_SUITE("delegate")

// TODO adapt delegate test suite for a event_delegate test suite

TEST_CASE("tst")
{
    using rome::delegates::delegate;
    using rome::delegates::make_delegate;

    struct B {
        int foo(float f)
        {
            CHECK(f == 1.1f);
            return 2;
        }
    };
    B b;
    auto d = make_delegate<decltype(&B::foo), &B::foo>(b);
    CHECK(d(1.1f) == 2);
}

TEST_CASE("null initialized event_delegate")
{
    using rome::delegates::event_delegate;
    using rome::delegates::make_event_delegate;

    testVal.reset();

    event_delegate<void(int, const bool &)> ed;

    REQUIRE(ed.isSet() == false);
    REQUIRE(ed == false);
    REQUIRE(!ed == true);

    SUBCASE("unlinked call")
    {
        ed(1, true);
        REQUIRE(ed.isSet() == false);
        REQUIRE(ed == false);
        REQUIRE(!ed == true);
    }

    SUBCASE("calls function")
    {
        const auto tmp = make_event_delegate<decltype(&foo), &foo>();

        REQUIRE(ed != tmp);

        ed = tmp;

        REQUIRE(ed.isSet() == true);
        REQUIRE(ed == true);
        REQUIRE(!ed == false);
        REQUIRE(ed == tmp);

        ed(5, true);

        REQUIRE(testVal.val == 5);

        ed(-1, false);

        REQUIRE(testVal.val == 6);
    }
    SUBCASE("calls functor")
    {
        Functor functor;
        ed = event_delegate<void(int, const bool &)>::create(
            functor); // TODO: add make_event_delegate for functors!

        REQUIRE(ed.isSet() == true);
        REQUIRE(ed == true);
        REQUIRE(!ed == false);

        ed(5, true);

        REQUIRE(functor.val == 5);

        ed(-1, false);

        REQUIRE(functor.val == 6);
    }
}
