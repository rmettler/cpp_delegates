//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <rome/delegate.hpp>
#include <test/common_delegate_checks.hpp>


#if __GNUC__ >= 12
DOCTEST_GCC_SUPPRESS_WARNING("-Wmismatched-new-delete")
// GCC seems to have problems to detect that for both overloaded new and delete of the mocks the
// same global allocator/deallocator are used and raises a false positive warning.
#endif

template<typename Signature>
struct DummyFunctor;

template<typename Ret, typename... Args>
struct DummyFunctor<Ret(Args...)> {
    auto operator()(Args...) {
        return Ret{};
    }
};

template<typename... Args>
struct DummyFunctor<void(Args...)> {
    void operator()(Args...) {
    }
};


// clang-format off
using test_vector = std::tuple<
        rome::delegate<bool(int), rome::target_is_expected>,
        rome::delegate<bool(int), rome::target_is_mandatory>,
        rome::delegate<void(int), rome::target_is_expected>,
        rome::delegate<void(int), rome::target_is_mandatory>,
        rome::delegate<void(int), rome::target_is_optional>,
    rome::fwd_delegate<void(int), rome::target_is_expected>,
    rome::fwd_delegate<void(int), rome::target_is_mandatory>,
    rome::fwd_delegate<void(int), rome::target_is_optional>
>;
// clang-format on

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters,misc-use-anonymous-namespace,readability-function-size,readability-function-cognitive-complexity)
TEST_CASE_TEMPLATE_DEFINE("Create delegate with an assigned target. ", Delegate, create_assigned) {
    using Sig = test::delegate_signature_t<Delegate>;

    SUBCASE("Target: Function") {
        using test::mockedFunction;
        STATIC_REQUIRE(noexcept(Delegate::template create<&mockedFunction<Sig>>()));
        const Delegate dgt = Delegate::template create<&mockedFunction<Sig>>();
        CHECK(test::isObservedAsAssigned(dgt));
        CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
    }
    SUBCASE("Target: Non-static member function") {
        using test::TargetClass;
        using test::targetObject;
        STATIC_REQUIRE(
            noexcept(Delegate::template create<TargetClass<Sig>, &TargetClass<Sig>::mockedMethod>(
                targetObject<Sig>)));
        const Delegate dgt =
            Delegate::template create<TargetClass<Sig>, &TargetClass<Sig>::mockedMethod>(
                targetObject<Sig>);
        CHECK(test::isObservedAsAssigned(dgt));
        CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
    }
    SUBCASE("Target: Non-static const member function") {
        using test::TargetClass;
        using test::targetConstObject;
        STATIC_REQUIRE(noexcept(
            Delegate::template create<TargetClass<Sig>, &TargetClass<Sig>::mockedConstMethod>(
                targetConstObject<Sig>)));
        const Delegate dgt =
            Delegate::template create<TargetClass<Sig>, &TargetClass<Sig>::mockedConstMethod>(
                targetConstObject<Sig>);
        CHECK(test::isObservedAsAssigned(dgt));
        CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
    }
    SUBCASE("Target: Small object optimizable functor (needs no heap allocation)") {
        using test::targetMock;
        using Functor = test::ObjectOptimizableFunctor<Sig>;
        SUBCASE("Construct by moving the functor") {
            SUBCASE("Is noexcept") {
                STATIC_REQUIRE(noexcept(Delegate{std::declval<Functor>()}));
                STATIC_REQUIRE(noexcept(Delegate{std::move(std::declval<Functor>())}));
                STATIC_REQUIRE(noexcept(std::declval<Delegate&>() = std::declval<Functor>()));
                STATIC_REQUIRE(
                    noexcept(std::declval<Delegate&>() = std::move(std::declval<Functor>())));
                STATIC_REQUIRE(noexcept(Delegate::create(std::declval<Functor>())));
                STATIC_REQUIRE(noexcept(Delegate::create(std::move(std::declval<Functor>()))));
            }
            SUBCASE("Construction succeeds") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), moveConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).TIMES(2).IN_SEQUENCE(seq);
                Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    const Delegate dgt{std::move(mockedFunctor)};
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    // NOLINTNEXTLINE(bugprone-use-after-move)
                    dgt = std::move(mockedFunctor);
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
                SUBCASE("By using the factory method") {
                    // NOLINTNEXTLINE(bugprone-use-after-move)
                    const Delegate dgt = Delegate::create(std::move(mockedFunctor));
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
            }
        }
        SUBCASE("Construct by copying the functor") {
            SUBCASE("Is noexcept if the functor is noexcept copy-constructible") {
                static_assert(!std::is_nothrow_copy_constructible<Functor>{}, "precondition");
                STATIC_REQUIRE(!noexcept(Delegate{std::declval<Functor&>()}));
                STATIC_REQUIRE(!noexcept(Delegate{std::declval<const Functor&>()}));
                STATIC_REQUIRE(!noexcept(std::declval<Delegate&>() = std::declval<Functor&>()));
                STATIC_REQUIRE(
                    !noexcept(std::declval<Delegate&>() = std::declval<const Functor&>()));
                STATIC_REQUIRE(!noexcept(Delegate::create(std::declval<Functor&>())));
                STATIC_REQUIRE(!noexcept(Delegate::create(std::declval<const Functor&>())));
                // NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
                struct NoexceptFunctor {
                    NoexceptFunctor(const NoexceptFunctor&) noexcept;
                    auto operator()(test::delegate_argument_type_t<0, Delegate>)
                        -> test::delegate_return_type_t<Delegate>;
                };
                static_assert(
                    std::is_nothrow_copy_constructible<NoexceptFunctor>{}, "precondition");
                STATIC_REQUIRE(noexcept(Delegate{std::declval<NoexceptFunctor&>()}));
                STATIC_REQUIRE(noexcept(Delegate{std::declval<const NoexceptFunctor&>()}));
                STATIC_REQUIRE(
                    noexcept(std::declval<Delegate&>() = std::declval<NoexceptFunctor&>()));
                STATIC_REQUIRE(
                    noexcept(std::declval<Delegate&>() = std::declval<const NoexceptFunctor&>()));
                STATIC_REQUIRE(noexcept(Delegate::create(std::declval<NoexceptFunctor&>())));
                STATIC_REQUIRE(noexcept(Delegate::create(std::declval<const NoexceptFunctor&>())));
            }
            SUBCASE("Construction succeeds") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), copyConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).TIMES(2).IN_SEQUENCE(seq);
                const Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    const Delegate dgt{mockedFunctor};
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    dgt          = mockedFunctor;
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
                SUBCASE("By using the factory method") {
                    const Delegate dgt = Delegate::create(mockedFunctor);
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
            }
            SUBCASE("Error while functor is copied") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), copyConstruct())
                    .THROW(std::runtime_error{"test"})
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                const Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    CHECK_THROWS_WITH_AS(Delegate{mockedFunctor}, "test", std::runtime_error);
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    CHECK_THROWS_WITH_AS((dgt = mockedFunctor), "test", std::runtime_error);
                }
                SUBCASE("By using the factory method") {
                    CHECK_THROWS_WITH_AS(
                        Delegate::create(mockedFunctor), "test", std::runtime_error);
                }
            }
        }
    }
    SUBCASE("Target: Too big functor for small object optimization (needs heap allocation)") {
        using test::targetMock;
        using Functor = test::TooBigFunctor<Sig>;
        SUBCASE("Construct by moving the functor") {
            SUBCASE("Is not noexcept") {
                STATIC_REQUIRE(!noexcept(Delegate{std::declval<Functor>()}));
                STATIC_REQUIRE(!noexcept(Delegate{std::move(std::declval<Functor>())}));
                STATIC_REQUIRE(!noexcept(std::declval<Delegate&>() = std::declval<Functor>()));
                STATIC_REQUIRE(
                    !noexcept(std::declval<Delegate&>() = std::move(std::declval<Functor>())));
                STATIC_REQUIRE(!noexcept(Delegate::create(std::declval<Functor>())));
                STATIC_REQUIRE(!noexcept(Delegate::create(std::move(std::declval<Functor>()))));
            }
            SUBCASE("Construction succeeds") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), new_()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), moveConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), delete_()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    const Delegate dgt{std::move(mockedFunctor)};
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    // NOLINTNEXTLINE(bugprone-use-after-move)
                    dgt = std::move(mockedFunctor);
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
                SUBCASE("By using the factory method") {
                    // NOLINTNEXTLINE(bugprone-use-after-move)
                    const Delegate dgt = Delegate::create(std::move(mockedFunctor));
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
            }
            SUBCASE("Error during heap allocation") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), new_()).THROW(std::bad_alloc{}).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    CHECK_THROWS_AS(Delegate{std::move(mockedFunctor)}, std::bad_alloc);
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    // NOLINTNEXTLINE(bugprone-use-after-move)
                    CHECK_THROWS_AS((dgt = std::move(mockedFunctor)), std::bad_alloc);
                }
                SUBCASE("By using the factory method") {
                    // NOLINTNEXTLINE(bugprone-use-after-move)
                    CHECK_THROWS_AS(Delegate::create(std::move(mockedFunctor)), std::bad_alloc);
                }
            }
        }
        SUBCASE("Construct by copying the functor") {
            SUBCASE("Is not noexcept") {
                STATIC_REQUIRE(!noexcept(Delegate{std::declval<Functor&>()}));
                STATIC_REQUIRE(!noexcept(Delegate{std::declval<const Functor&>()}));
                STATIC_REQUIRE(!noexcept(std::declval<Delegate&>() = std::declval<Functor&>()));
                STATIC_REQUIRE(
                    !noexcept(std::declval<Delegate&>() = std::declval<const Functor&>()));
                STATIC_REQUIRE(!noexcept(Delegate::create(std::declval<Functor&>())));
                STATIC_REQUIRE(!noexcept(Delegate::create(std::declval<const Functor&>())));
            }
            SUBCASE("Construction succeeds") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), new_()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), copyConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), delete_()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                const Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    const Delegate dgt{mockedFunctor};
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    dgt          = mockedFunctor;
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
                SUBCASE("By using the factory method") {
                    const Delegate dgt = Delegate::create(mockedFunctor);
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
            }
            SUBCASE("Error during heap allocation") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), new_()).THROW(std::bad_alloc{}).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                const Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    CHECK_THROWS_AS(Delegate{mockedFunctor}, std::bad_alloc);
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    CHECK_THROWS_AS((dgt = mockedFunctor), std::bad_alloc);
                }
                SUBCASE("By using the factory method") {
                    CHECK_THROWS_AS(Delegate::create(mockedFunctor), std::bad_alloc);
                }
            }
            SUBCASE("Error while functor is copied") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), new_()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), copyConstruct())
                    .THROW(std::runtime_error{"test"})
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), delete_()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                const Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    CHECK_THROWS_WITH_AS(Delegate{mockedFunctor}, "test", std::runtime_error);
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    CHECK_THROWS_WITH_AS((dgt = mockedFunctor), "test", std::runtime_error);
                }
                SUBCASE("By using the factory method") {
                    CHECK_THROWS_WITH_AS(
                        Delegate::create(mockedFunctor), "test", std::runtime_error);
                }
            }
        }
    }
    SUBCASE("Target: Functor with an alignment that cannot be small object optimized (needs heap "
            "allocation)") {
        using test::targetMock;
        using Functor = test::BadAlignedFunctor<Sig>;
        SUBCASE("Construct by moving the functor") {
            SUBCASE("Is not noexcept") {
                STATIC_REQUIRE(!noexcept(Delegate{std::declval<Functor>()}));
                STATIC_REQUIRE(!noexcept(Delegate{std::move(std::declval<Functor>())}));
                STATIC_REQUIRE(!noexcept(std::declval<Delegate&>() = std::declval<Functor>()));
                STATIC_REQUIRE(
                    !noexcept(std::declval<Delegate&>() = std::move(std::declval<Functor>())));
                STATIC_REQUIRE(!noexcept(Delegate::create(std::declval<Functor>())));
                STATIC_REQUIRE(!noexcept(Delegate::create(std::move(std::declval<Functor>()))));
            }
            SUBCASE("Construction succeeds") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), new_()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), moveConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), delete_()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    const Delegate dgt{std::move(mockedFunctor)};
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    // NOLINTNEXTLINE(bugprone-use-after-move)
                    dgt = std::move(mockedFunctor);
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
                SUBCASE("By using the factory method") {
                    // NOLINTNEXTLINE(bugprone-use-after-move)
                    const Delegate dgt = Delegate::create(std::move(mockedFunctor));
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
            }
            SUBCASE("Error during heap allocation") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), new_()).THROW(std::bad_alloc{}).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    CHECK_THROWS_AS(Delegate{std::move(mockedFunctor)}, std::bad_alloc);
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    // NOLINTNEXTLINE(bugprone-use-after-move)
                    CHECK_THROWS_AS((dgt = std::move(mockedFunctor)), std::bad_alloc);
                }
                SUBCASE("By using the factory method") {
                    // NOLINTNEXTLINE(bugprone-use-after-move)
                    CHECK_THROWS_AS(Delegate::create(std::move(mockedFunctor)), std::bad_alloc);
                }
            }
        }
        SUBCASE("Construct by copying the functor") {
            SUBCASE("Is not noexcept") {
                STATIC_REQUIRE(!noexcept(Delegate{std::declval<Functor&>()}));
                STATIC_REQUIRE(!noexcept(Delegate{std::declval<const Functor&>()}));
                STATIC_REQUIRE(!noexcept(std::declval<Delegate&>() = std::declval<Functor&>()));
                STATIC_REQUIRE(
                    !noexcept(std::declval<Delegate&>() = std::declval<const Functor&>()));
                STATIC_REQUIRE(!noexcept(Delegate::create(std::declval<Functor&>())));
                STATIC_REQUIRE(!noexcept(Delegate::create(std::declval<const Functor&>())));
            }
            SUBCASE("Construction succeeds") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), new_()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), copyConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), delete_()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                const Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    const Delegate dgt{mockedFunctor};
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    dgt          = mockedFunctor;
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
                SUBCASE("By using the factory method") {
                    const Delegate dgt = Delegate::create(mockedFunctor);
                    CHECK(test::isObservedAsAssigned(dgt));
                    CHECK(test::callingDelegateCallsMockedTarget<>(dgt));
                }
            }
            SUBCASE("Error during heap allocation") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), new_()).THROW(std::bad_alloc{}).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                const Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    CHECK_THROWS_AS(Delegate{mockedFunctor}, std::bad_alloc);
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    CHECK_THROWS_AS((dgt = mockedFunctor), std::bad_alloc);
                }
                SUBCASE("By using the factory method") {
                    CHECK_THROWS_AS(Delegate::create(mockedFunctor), std::bad_alloc);
                }
            }
            SUBCASE("Error while functor is copied") {
                trompeloeil::sequence seq;
                REQUIRE_CALL((targetMock<Sig>), defaultConstruct()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), new_()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), copyConstruct())
                    .THROW(std::runtime_error{"test"})
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), delete_()).IN_SEQUENCE(seq);
                REQUIRE_CALL((targetMock<Sig>), destruct()).IN_SEQUENCE(seq);
                const Functor mockedFunctor;
                SUBCASE("By using direct construction") {
                    CHECK_THROWS_WITH_AS(Delegate{mockedFunctor}, "test", std::runtime_error);
                }
                SUBCASE("By using the assignment operator") {
                    Delegate dgt = DummyFunctor<Sig>{};
                    CHECK_THROWS_WITH_AS((dgt = mockedFunctor), "test", std::runtime_error);
                }
                SUBCASE("By using the factory method") {
                    CHECK_THROWS_WITH_AS(
                        Delegate::create(mockedFunctor), "test", std::runtime_error);
                }
            }
        }
    }
}
TEST_CASE_TEMPLATE_APPLY(create_assigned, test_vector);
