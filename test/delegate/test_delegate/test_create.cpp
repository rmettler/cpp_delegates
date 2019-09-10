//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//


#include <doctest.h>

#include <rome/delegate.hpp>
#include <type_traits>

#include "checks.hpp"

namespace test_rome_delegate {

TEST_SUITE_BEGIN("header file: rome/delegate.hpp");

bool foo(int i) {
    return i > 0;
}

TEST_CASE("rome::delegate - create") {
    using rome::delegate;
    auto d = delegate<bool(int)>::create<&foo>();
    test_rome_delegate::checkNotEmpty(d);
    CHECK(d(1) == true);
    CHECK(d(-1) == false);
}

struct Calls {
    int defaultConstructions;
    int copyConstructions;
    int moveConstructions;
    int moveAssigns;
    int destroys;
    int calls;
    int news;
    int deletes;
    void reset() {
        defaultConstructions = 0;
        copyConstructions    = 0;
        moveConstructions    = 0;
        moveAssigns          = 0;
        destroys             = 0;
        calls                = 0;
        news                 = 0;
        deletes              = 0;
    }
    void checkWith(const Calls& exp) {
        CHECK(defaultConstructions == exp.defaultConstructions);
        CHECK(copyConstructions == exp.copyConstructions);
        CHECK(moveConstructions == exp.moveConstructions);
        CHECK(moveAssigns == exp.moveAssigns);
        CHECK(destroys == exp.destroys);
        CHECK(calls == exp.calls);
        CHECK(news == exp.news);
        CHECK(deletes == exp.deletes);
    }
};

struct SmallFunctor {
    static Calls calls;
    Calls& calls_;
    SmallFunctor() : calls_{calls} {
        ++calls.defaultConstructions;
    }
    SmallFunctor(const SmallFunctor&) : calls_{calls} {
        ++calls.copyConstructions;
    }
    SmallFunctor(SmallFunctor&&) : calls_{calls} {
        ++calls.moveConstructions;
    }
    SmallFunctor& operator=(SmallFunctor&&) {
        ++calls.moveAssigns;
        return *this;
    }
    ~SmallFunctor() {
        ++calls.destroys;
    }
    static void* operator new(std::size_t sz) {
        ++calls.news;
        return ::operator new(sz);
    }
    static void operator delete(void* ptr) {
        ++calls.deletes;
        ::operator delete(ptr);
    }
    void operator()() {
        ++calls.calls;
    }
};
Calls SmallFunctor::calls = {};

struct BiggerFunctor {
    static Calls calls;
    Calls& calls1_;
    Calls& calls2_;
    BiggerFunctor() : calls1_{calls}, calls2_{calls} {
        ++calls.defaultConstructions;
    }
    BiggerFunctor(const BiggerFunctor&) : calls1_{calls}, calls2_{calls} {
        ++calls.copyConstructions;
    }
    BiggerFunctor(BiggerFunctor&&) : calls1_{calls}, calls2_{calls} {
        ++calls.moveConstructions;
    }
    BiggerFunctor& operator=(BiggerFunctor&&) {
        ++calls.moveAssigns;
        return *this;
    }
    ~BiggerFunctor() {
        ++calls.destroys;
    }
    static void* operator new(std::size_t sz) {
        ++calls.news;
        return ::operator new(sz);
    }
    static void operator delete(void* ptr) {
        ++calls.deletes;
        ::operator delete(ptr);
    }
    void operator()() {
        ++calls.calls;
    }
};
Calls BiggerFunctor::calls = {};

TEST_CASE("target_t") {
    // TODO: do this with delegate instead!
    using target_t = rome::detail::delegate::target_t<void()>;
    Calls calls;
    calls.reset();
    SUBCASE("functor with small buffer optimization") {
        SmallFunctor::calls.reset();
        {
            SmallFunctor sf;
            ++calls.defaultConstructions;
            SmallFunctor::calls.checkWith(calls);
            {
                auto t1 = target_t::create(sf);
                ++calls.copyConstructions;
                ++calls.destroys;  // the temporary copy
                ++calls.moveConstructions;
                SmallFunctor::calls.checkWith(calls);

                t1();
                ++calls.calls;
                SmallFunctor::calls.checkWith(calls);
                {
                    auto t2{std::move(t1)};
                    SmallFunctor::calls.checkWith(calls);

                    t1();
                    SmallFunctor::calls.checkWith(calls);

                    t2();
                    ++calls.calls;
                    SmallFunctor::calls.checkWith(calls);
                }
                ++calls.destroys; // destroys t2
                SmallFunctor::calls.checkWith(calls);
            }
            SmallFunctor::calls.checkWith(calls);
        }
        ++calls.destroys; // destroys sf
        SmallFunctor::calls.checkWith(calls);
    }
    SUBCASE("functor with small buffer optimization") {
        BiggerFunctor::calls.reset();
        {
            BiggerFunctor sf;
            ++calls.defaultConstructions;
            BiggerFunctor::calls.checkWith(calls);
            {
                auto t1 = target_t::create(sf);
                ++calls.copyConstructions;
                ++calls.destroys;
                ++calls.news;
                ++calls.moveConstructions;
                BiggerFunctor::calls.checkWith(calls);

                t1();
                ++calls.calls;
                BiggerFunctor::calls.checkWith(calls);
                {
                    auto t2{std::move(t1)};
                    BiggerFunctor::calls.checkWith(calls);

                    t1();
                    BiggerFunctor::calls.checkWith(calls);

                    t2();
                    ++calls.calls;
                    BiggerFunctor::calls.checkWith(calls);
                }
                ++calls.destroys; // destroys t2
                ++calls.deletes; // deletes t2
                BiggerFunctor::calls.checkWith(calls);
            }
            BiggerFunctor::calls.checkWith(calls);
        }
        ++calls.destroys; // destroys sf
        BiggerFunctor::calls.checkWith(calls);
    }
}

TEST_SUITE_END();

}  // namespace test_rome_delegate
