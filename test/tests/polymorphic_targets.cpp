//
// Project: C++ delegates
//
// Copyright Roger Mettler 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <rome/delegate.hpp>
#include <test/common_delegate_checks.hpp>


// NOLINTBEGIN(cppcoreguidelines-special-member-functions)
class A {
    char value_ = 'A';

  public:
    virtual ~A() = default;
    virtual auto value() const -> char {
        return value_;
    }
};

class B {
    char value_ = 'B';

  public:
    virtual auto value() const -> char {
        return value_;
    }
    virtual ~B() = default;
};

class Derived : public A, public B {
    char value_ = 'D';

  public:
    auto value() const -> char override {
        return value_;
    }
};
// NOLINTEND(cppcoreguidelines-special-member-functions)

// clang-format off
using test_vector_polymorphic = std::tuple<
        rome::delegate<char(), rome::target_is_expected>,
        rome::delegate<char(), rome::target_is_mandatory>,
        rome::delegate<char(), rome::target_is_expected>,
        rome::delegate<char(), rome::target_is_mandatory>
>;
// clang-format on

// NOLINTNEXTLINE(readability-function-cognitive-complexity,bugprone-easily-swappable-parameters,misc-use-anonymous-namespace)
TEST_CASE_TEMPLATE_DEFINE(
    "Virtual member functions work with delegates.", Delegate, create_from_polymorphic) {
    const A a{};
    const B b{};
    const Derived derived{};
    {
        const auto dgt = Delegate::template create<A, &A::value>(a);
        CHECK(dgt() == 'A');
    }
    {
        const auto dgt = Delegate::template create<B, &B::value>(b);
        CHECK(dgt() == 'B');
    }
    {
        const auto dgt = Delegate::template create<Derived, &Derived::value>(derived);
        CHECK(dgt() == 'D');
    }
    {
        const A& aRef  = derived;
        const auto dgt = Delegate::template create<A, &A::value>(aRef);
        CHECK(dgt() == 'D');
    }
    {
        const auto dgt = Delegate::template create<A, &A::value>(derived);
        CHECK(dgt() == 'D');
    }
    {
        const B& bRef  = derived;
        const auto dgt = Delegate::template create<B, &B::value>(bRef);
        CHECK(dgt() == 'D');
    }
    {
        const auto dgt = Delegate::template create<B, &B::value>(derived);
        CHECK(dgt() == 'D');
    }
}
TEST_CASE_TEMPLATE_APPLY(create_from_polymorphic, test_vector_polymorphic);
