//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//


#include <doctest.h>

#include <functional>
#include <rome/detail/delegate_target.hpp>
#include <type_traits>

namespace test_rome_delegate {

TEST_SUITE_BEGIN("header file: rome/detail/delegate_target.hpp");

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
    bool operator==(const Calls& rhs) {
        CHECK(defaultConstructions == rhs.defaultConstructions);
        CHECK(copyConstructions == rhs.copyConstructions);
        CHECK(moveConstructions == rhs.moveConstructions);
        CHECK(moveAssigns == rhs.moveAssigns);
        CHECK(destroys == rhs.destroys);
        CHECK(calls == rhs.calls);
        CHECK(news == rhs.news);
        CHECK(deletes == rhs.deletes);
        return (defaultConstructions == rhs.defaultConstructions)
               && (copyConstructions == rhs.copyConstructions)
               && (moveConstructions == rhs.moveConstructions) && (moveAssigns == rhs.moveAssigns)
               && (destroys == rhs.destroys) && (calls == rhs.calls) && (news == rhs.news)
               && (deletes == rhs.deletes);
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

struct BadAlignedFunctor {
    static Calls calls;
    alignas(2 * sizeof(void*)) Calls& calls_;
    BadAlignedFunctor() : calls_{calls} {
        ++calls.defaultConstructions;
    }
    BadAlignedFunctor(const BadAlignedFunctor&) : calls_{calls} {
        ++calls.copyConstructions;
    }
    BadAlignedFunctor(BadAlignedFunctor&&) : calls_{calls} {
        ++calls.moveConstructions;
    }
    BadAlignedFunctor& operator=(BadAlignedFunctor&&) {
        ++calls.moveAssigns;
        return *this;
    }
    ~BadAlignedFunctor() {
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
Calls BadAlignedFunctor::calls = {};

TEST_CASE("delegate_target") {
    // TODO: do this with delegate instead!
    using target_t = rome::detail::delegate_target<void()>;
    Calls calls;
    calls.reset();
    SUBCASE("functor WITH small buffer optimization") {
        using Functor = SmallFunctor;
        Functor::calls.reset();
        // check proper test setup
        CHECK(sizeof(Functor) <= sizeof(void*));
        CHECK(alignof(Functor) <= sizeof(void*));
        CHECK((Functor::calls == calls));
        {
            Functor f{};
            ++calls.defaultConstructions;
            CHECK((Functor::calls == calls));

            auto t1 = target_t::create(f);
            // copy construction and destruction during the temporary object f is passed
            ++calls.copyConstructions;
            ++calls.destroys;
            // move construction used to initialize buffer_
            ++calls.moveConstructions;
            CHECK((Functor::calls == calls));
            CHECK(static_cast<bool>(t1));

            t1();
            ++calls.calls;
            CHECK((Functor::calls == calls));
            {
                auto t2{std::move(t1)};
                CHECK((Functor::calls == calls));
                CHECK(static_cast<bool>(t1));
                CHECK(static_cast<bool>(t2));

                t1();
                ++calls.calls;
                CHECK((Functor::calls == calls));

                t1 = target_t{nullptr, target_t::no_call, target_t::no_delete};
                t1();
                CHECK((Functor::calls == calls));

                t2();
                ++calls.calls;
                CHECK((Functor::calls == calls));
            }
            ++calls.destroys;  // destroys t2
            CHECK((Functor::calls == calls));
        }
        ++calls.destroys;  // destroys f
        CHECK((Functor::calls == calls));
    }
    SUBCASE("functor too big for small buffer optimization") {
        using Functor = BiggerFunctor;
        Functor::calls.reset();
        // check proper test setup
        CHECK(sizeof(Functor) > sizeof(void*));
        CHECK(alignof(Functor) <= sizeof(void*));
        CHECK((Functor::calls == calls));
        {
            Functor f;
            ++calls.defaultConstructions;
            CHECK((Functor::calls == calls));

            auto t1 = target_t::create(f);
            // copy construction and destruction during the temporary object f is passed
            ++calls.copyConstructions;
            ++calls.destroys;
            // move construction used to initialize buffer_
            ++calls.news;
            ++calls.moveConstructions;
            CHECK((Functor::calls == calls));

            t1();
            ++calls.calls;
            CHECK((Functor::calls == calls));
            {
                auto t2{std::move(t1)};
                CHECK((Functor::calls == calls));

                t1();
                ++calls.calls;
                CHECK((Functor::calls == calls));

                t1 = target_t{nullptr, target_t::no_call, target_t::no_delete};
                t1();
                CHECK((Functor::calls == calls));

                t2();
                ++calls.calls;
                CHECK((Functor::calls == calls));
            }
            ++calls.destroys;  // destroys t2
            ++calls.deletes;   // deletes t2
            CHECK((Functor::calls == calls));
        }
        ++calls.destroys;  // destroys f
        CHECK((Functor::calls == calls));
    }
    SUBCASE("functor too badly aligned for small buffer optimization") {
        using Functor = BadAlignedFunctor;
        Functor::calls.reset();
        // check proper test setup
        CHECK(alignof(Functor) > sizeof(void*));
        CHECK((Functor::calls == calls));
        {
            Functor f;
            ++calls.defaultConstructions;
            CHECK((Functor::calls == calls));

            auto t1 = target_t::create(f);
            // copy construction and destruction during the temporary object f is passed
            ++calls.copyConstructions;
            ++calls.destroys;
            // move construction used to initialize buffer_
            ++calls.news;
            ++calls.moveConstructions;
            CHECK((Functor::calls == calls));

            t1();
            ++calls.calls;
            CHECK((Functor::calls == calls));
            {
                auto t2{std::move(t1)};
                CHECK((Functor::calls == calls));

                t1();
                ++calls.calls;
                CHECK((Functor::calls == calls));

                t1 = target_t{nullptr, target_t::no_call, target_t::no_delete};
                t1();
                CHECK((Functor::calls == calls));

                t2();
                ++calls.calls;
                CHECK((Functor::calls == calls));
            }
            ++calls.destroys;  // destroys t2
            ++calls.deletes;   // deletes t2
            CHECK((Functor::calls == calls));
        }
        ++calls.destroys;  // destroys f
        CHECK((Functor::calls == calls));
    }
}

TEST_SUITE_END();

}  // namespace test_rome_delegate
