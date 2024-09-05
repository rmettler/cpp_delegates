//
// Project: C++ delegates
//
// Copyright Roger Mettler 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//
// Provides reusable test fragments to:
//  - create a delegate (with an assigned target or empty) and check for the expected behavior
//  - create the expectation for the destruction of certain target
//  - check whether a delegate is observed as empty
//  - check whether a delegate is observed as assigned to a target
//  - chech whether a delegate behaves as if empty when called
//  - chech whether a delegate behaves as if a certain target is assigned when called

#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil/sequence.hpp>

#include <rome/delegate.hpp>
#include <test/delegate_traits.hpp>
#include <test/doctest_extensions.hpp>
#include <test/target_mocks.hpp>


namespace test {

namespace target {
    // Tag for no target (empty delegate).
    struct none {};
    // Tag for a free function or static member function.
    struct function {};
    // Tag for a non-static non-const member function.
    struct method {};
    // Tag for a non-static const member function.
    struct const_method {};
    // Tag for a functor that is small object optimizable within a delegate (no heap allocation
    // needed).
    struct object_optimizable_functor {};
    // Tag for a functor that is too big for small object optimization within a delegate (heap
    // allocation needed).
    struct too_big_functor {};
    // Tag for a functor with an alignment that cannot be small object optimized within a delegate
    // (heap allocation needed).
    struct bad_aligned_functor {};
}  // namespace target

namespace detail {
    template<typename Delegate, size_t N>
    struct delegate_creator {
        using Sig = delegate_signature_t<Delegate>;
        auto operator()(const target::none&) -> Delegate {
            return Delegate{};
        }
        auto operator()(const target::function&) -> Delegate {
            return Delegate::template create<&mockedFunction<Sig, N>>();
        }
        auto operator()(const target::method&) -> Delegate {
            using Cls = TargetClass<Sig, N>;
            return Delegate::template create<Cls, &Cls::mockedMethod>(targetObject<Sig, N>);
        }
        auto operator()(const target::const_method&) -> Delegate {
            using Cls = TargetClass<Sig, N>;
            return Delegate::template create<Cls, &Cls::mockedConstMethod>(
                targetConstObject<Sig, N>);
        }
        auto operator()(const target::object_optimizable_functor&) -> Delegate {
            trompeloeil::sequence seq;
            REQUIRE_CALL((targetMock<Sig, N>), defaultConstruct())
                .IN_SEQUENCE(seq);  // temporary 'mockedFunctor'
            REQUIRE_CALL((targetMock<Sig, N>), moveConstruct()).IN_SEQUENCE(seq);
            REQUIRE_CALL((targetMock<Sig, N>), destruct())
                .IN_SEQUENCE(seq);  // temporary 'mockedFunctor'
            ObjectOptimizableFunctor<Sig, N> mockedFunctor;
            return Delegate::create(std::move(mockedFunctor));
        }
        template<typename Functor>
        static auto createHeapAllocatedFunctor() -> Delegate {
            trompeloeil::sequence seq;
            REQUIRE_CALL((targetMock<Sig, N>), defaultConstruct())
                .IN_SEQUENCE(seq);  // temporary 'mockedFunctor'
            REQUIRE_CALL((targetMock<Sig, N>), new_()).IN_SEQUENCE(seq);
            REQUIRE_CALL((targetMock<Sig, N>), moveConstruct()).IN_SEQUENCE(seq);
            REQUIRE_CALL((targetMock<Sig, N>), destruct())
                .IN_SEQUENCE(seq);  // temporary 'mockedFunctor'
            Functor mockedFunctor;
            return Delegate::create(std::move(mockedFunctor));
        }
        auto operator()(const target::too_big_functor&) -> Delegate {
            return createHeapAllocatedFunctor<TooBigFunctor<Sig, N>>();
        }
        auto operator()(const target::bad_aligned_functor&) -> Delegate {
            return createHeapAllocatedFunctor<BadAlignedFunctor<Sig, N>>();
        }
    };
}  // namespace detail

// Create a delegate of the type given by 'Delegate' with the mocked target set by 'TargetType'.
// As a possible 'TargetType' see the structs in the namespace 'target::'.
// The number 'N' identifies by which global mock object 'targetMock<Sig, N>' the calls to the
// target are controlled.
template<typename Delegate, typename TargetType, size_t N = 0>
auto createDelegateWithMockedTarget() -> Delegate {
    return detail::delegate_creator<Delegate, N>{}(TargetType{});
}


// Create a 'std::unique_ptr<trompeloeil::expectation>' that checks at the end of its lifetime
// whether the target registered with the global 'targetMock<CallSignature, N>' was destroyed as
// expected for the target of type 'TargetType'.
template<typename CallSignature, typename TargetType, size_t N = 0,
    std::enable_if_t<std::is_same<TargetType, target::none>{}
                         || std::is_same<TargetType, target::function>{}
                         || std::is_same<TargetType, target::method>{}
                         || std::is_same<TargetType, target::const_method>{},
        int> = 0>
auto expectDestroyTargetAtEndOfScope() -> auto {
    return std::unique_ptr<void*>{};
}

template<typename CallSignature, typename TargetType, size_t N = 0,
    std::enable_if_t<std::is_same<TargetType, target::object_optimizable_functor>{}, int> = 0>
auto expectDestroyTargetAtEndOfScope() -> auto {
    return NAMED_REQUIRE_CALL((targetMock<CallSignature, N>), destruct());
}

template<typename CallSignature, typename TargetType, size_t N = 0,
    std::enable_if_t<std::is_same<TargetType, target::too_big_functor>{}
                         || std::is_same<TargetType, target::bad_aligned_functor>{},
        int> = 0>
auto expectDestroyTargetAtEndOfScope() -> auto {
    // clang-format off
    auto t = std::make_tuple(
        NAMED_REQUIRE_CALL((targetMock<CallSignature, N>), destruct()),
        NAMED_REQUIRE_CALL((targetMock<CallSignature, N>), delete_()));
    //clang-format on
    return std::make_unique<decltype(t)>(std::move(t));
}


// Check whether given delegate is observed as empty.
template<typename Delegate>
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto isObservedAsEmpty(const Delegate& dgt) -> bool {
    // clang-format off
    return CHECK_R(dgt == nullptr)
            && CHECK_R(nullptr == dgt)
            && CHECK_R(!(dgt != nullptr))
            && CHECK_R(!(nullptr != dgt))
            && CHECK_R(!static_cast<bool>(dgt))
            && CHECK_R(!dgt);
    // clang-format on
}


// Check whether given delegate is observed as assigned to a target.
template<typename Delegate>
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto isObservedAsAssigned(const Delegate& dgt) -> bool {
    // clang-format off
    return CHECK_R(dgt != nullptr)
            && CHECK_R(nullptr != dgt)
            && CHECK_R(!(dgt == nullptr))
            && CHECK_R(!(nullptr == dgt))
            && CHECK_R(static_cast<bool>(dgt))
            && CHECK_R(!!dgt);
    // clang-format on
}


// Call given delegate and check whether it behaves as expected for an empty delegate of set
// 'Behavior'.
template<typename Delegate, typename Behavior = delegate_behavior_t<Delegate>,
    std::enable_if_t<std::is_same<Behavior, rome::target_is_expected>{}
                         || std::is_same<Behavior, rome::target_is_mandatory>{},
        int> = 0>
auto callingDelegateLeadsToEmptyBehavior(const Delegate& dgt) -> bool {
    int arg = 42;  // NOLINT(misc-const-correctness)
    REQUIRE_THROWS_WITH_AS(dgt(arg), "rome::bad_delegate_call", rome::bad_delegate_call);
    return true;
}

template<typename Delegate, typename Behavior = delegate_behavior_t<Delegate>,
    std::enable_if_t<std::is_same<Behavior, rome::target_is_optional>{}, int> = 0>
auto callingDelegateLeadsToEmptyBehavior(const Delegate& dgt) -> bool {
    int arg = 42;  // NOLINT(misc-const-correctness)
    REQUIRE_NOTHROW(dgt(arg));
    return true;
}


namespace detail {
    template<size_t N>
    struct assigned_delegate_call_tester {
        template<typename Delegate,
            std::enable_if_t<std::is_same<void(int), delegate_signature_t<Delegate>>{}, int> = 0>
        auto operator()(const Delegate& dgt) const -> bool {
            REQUIRE_CALL((targetMock<delegate_signature_t<Delegate>, N>), call(42));
            REQUIRE_NOTHROW(dgt(42));
            return true;
        }

        template<typename Delegate,
            std::enable_if_t<std::is_same<void(int&), delegate_signature_t<Delegate>>{}, int> = 0>
        // NOLINTNEXTLINE(readability-function-cognitive-complexity)
        auto operator()(const Delegate& dgt) const -> bool {
            using trompeloeil::_;
            int arg = 42;  // NOLINT(misc-const-correctness)
            REQUIRE_CALL((targetMock<delegate_signature_t<Delegate>, N>), call(_))
                .WITH(_1 == 42)
                .SIDE_EFFECT(++_1);
            REQUIRE_NOTHROW(dgt(arg));
            return CHECK_R(arg == 43);
        }

        template<typename Delegate,
            std::enable_if_t<std::is_same<bool(int), delegate_signature_t<Delegate>>{}, int> = 0>
        auto operator()(const Delegate& dgt) const -> bool {
            REQUIRE_CALL((targetMock<delegate_signature_t<Delegate>, N>), call(42)).RETURN(true);
            return CHECK_R(dgt(42) == true);
        }

        template<typename Delegate,
            std::enable_if_t<std::is_same<bool(int&), delegate_signature_t<Delegate>>{}, int> = 0>
        // NOLINTNEXTLINE(readability-function-cognitive-complexity)
        auto operator()(const Delegate& dgt) const -> bool {
            using trompeloeil::_;
            int arg = 42;  // NOLINT(misc-const-correctness)
            REQUIRE_CALL((targetMock<delegate_signature_t<Delegate>, N>), call(_))
                .WITH(_1 == 42)
                .SIDE_EFFECT(++_1)
                .RETURN(true);
            return CHECK_R(dgt(arg) == true) && CHECK_R(arg == 43);
        }
    };
}  // namespace detail

// When called with a delegate, checks whether it behaves as expected for the target assigned.
// 'N' sets on which global 'targetMock<Signature, N>' the calls ar expected.
// E.g.: 'CHECK(callingDelegateCallsMockedTarget<0>(dgt));'
template<size_t N = 0>
constexpr detail::assigned_delegate_call_tester<N> callingDelegateCallsMockedTarget{};
}  // namespace test
