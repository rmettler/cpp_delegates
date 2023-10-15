//
// Project: C++ delegates
//
// Copyright Roger Mettler 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <rome/delegate.hpp>

#include <doctest/doctest.h>
#include <test/doctest_extensions.hpp>


// Types for testing
enum E : int { e_zero, e_one };
enum class EC : int { zero, one };
union U {
    int i;
};
struct C {
    int i;
};


// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
// NOLINTNEXTLINE(misc-use-anonymous-namespace,readability-function-cognitive-complexity,readability-function-size)
TEST_CASE("A data type is only detected as immutable if every referencing except moving into the "
          "function is declared const.") {
    using rome::detail::delegate::is_immutable_argument;

    // copied data
    STATIC_REQUIRE(is_immutable_argument<int>());
    STATIC_REQUIRE(is_immutable_argument<float>());
    STATIC_REQUIRE(is_immutable_argument<bool>());
    STATIC_REQUIRE(is_immutable_argument<E>());
    STATIC_REQUIRE(is_immutable_argument<EC>());
    STATIC_REQUIRE(is_immutable_argument<C>());
    STATIC_REQUIRE(!is_immutable_argument<void (C::*)()>());
    STATIC_REQUIRE(is_immutable_argument<void (C::*)() const>());
    STATIC_REQUIRE(is_immutable_argument<std::nullptr_t>());
    STATIC_REQUIRE(is_immutable_argument<void()>());
    STATIC_REQUIRE(is_immutable_argument<const int>());
    STATIC_REQUIRE(is_immutable_argument<const float>());
    STATIC_REQUIRE(is_immutable_argument<const bool>());
    STATIC_REQUIRE(is_immutable_argument<const E>());
    STATIC_REQUIRE(is_immutable_argument<const EC>());
    STATIC_REQUIRE(is_immutable_argument<const C>());
    STATIC_REQUIRE(is_immutable_argument<const std::nullptr_t>());
    STATIC_REQUIRE(!is_immutable_argument<void (C::*const)()>());
    STATIC_REQUIRE(is_immutable_argument<void (C::*const)() const>());

    // moved values
    STATIC_REQUIRE(is_immutable_argument<int&&>());
    STATIC_REQUIRE(is_immutable_argument<float&&>());
    STATIC_REQUIRE(is_immutable_argument<bool&&>());
    STATIC_REQUIRE(is_immutable_argument<E&&>());
    STATIC_REQUIRE(is_immutable_argument<EC&&>());
    STATIC_REQUIRE(is_immutable_argument<C&&>());
    STATIC_REQUIRE(is_immutable_argument<std::nullptr_t&&>());
    STATIC_REQUIRE(is_immutable_argument<void(&&)()>());

    // referenced by l-value reference
    STATIC_REQUIRE(is_immutable_argument<const int&>());
    STATIC_REQUIRE(is_immutable_argument<const float&>());
    STATIC_REQUIRE(is_immutable_argument<const bool&>());
    STATIC_REQUIRE(is_immutable_argument<const E&>());
    STATIC_REQUIRE(is_immutable_argument<const EC&>());
    STATIC_REQUIRE(is_immutable_argument<const C&>());
    STATIC_REQUIRE(is_immutable_argument<const std::nullptr_t&>());
    STATIC_REQUIRE(!is_immutable_argument<int&>());
    STATIC_REQUIRE(!is_immutable_argument<float&>());
    STATIC_REQUIRE(!is_immutable_argument<bool&>());
    STATIC_REQUIRE(!is_immutable_argument<E&>());
    STATIC_REQUIRE(!is_immutable_argument<EC&>());
    STATIC_REQUIRE(!is_immutable_argument<C&>());
    STATIC_REQUIRE(is_immutable_argument<std::nullptr_t&>());
    STATIC_REQUIRE(is_immutable_argument<void (&)()>());

    // referenced by pointer to value
    STATIC_REQUIRE(is_immutable_argument<const int*>());
    STATIC_REQUIRE(is_immutable_argument<const float*>());
    STATIC_REQUIRE(is_immutable_argument<const bool*>());
    STATIC_REQUIRE(is_immutable_argument<const E*>());
    STATIC_REQUIRE(is_immutable_argument<const EC*>());
    STATIC_REQUIRE(is_immutable_argument<const C*>());
    STATIC_REQUIRE(is_immutable_argument<const std::nullptr_t*>());
    STATIC_REQUIRE(is_immutable_argument<const int* const>());
    STATIC_REQUIRE(is_immutable_argument<const float* const>());
    STATIC_REQUIRE(is_immutable_argument<const bool* const>());
    STATIC_REQUIRE(is_immutable_argument<const E* const>());
    STATIC_REQUIRE(is_immutable_argument<const EC* const>());
    STATIC_REQUIRE(is_immutable_argument<const C* const>());
    STATIC_REQUIRE(is_immutable_argument<const std::nullptr_t* const>());
    STATIC_REQUIRE(!is_immutable_argument<int*>());
    STATIC_REQUIRE(!is_immutable_argument<float*>());
    STATIC_REQUIRE(!is_immutable_argument<bool*>());
    STATIC_REQUIRE(!is_immutable_argument<E*>());
    STATIC_REQUIRE(!is_immutable_argument<EC*>());
    STATIC_REQUIRE(!is_immutable_argument<C*>());
    STATIC_REQUIRE(is_immutable_argument<std::nullptr_t*>());
    STATIC_REQUIRE(!is_immutable_argument<int* const>());
    STATIC_REQUIRE(!is_immutable_argument<float* const>());
    STATIC_REQUIRE(!is_immutable_argument<bool* const>());
    STATIC_REQUIRE(!is_immutable_argument<E* const>());
    STATIC_REQUIRE(!is_immutable_argument<EC* const>());
    STATIC_REQUIRE(!is_immutable_argument<C* const>());
    STATIC_REQUIRE(is_immutable_argument<std::nullptr_t* const>());

    // referenced by C-style array
    STATIC_REQUIRE(is_immutable_argument<const int[]>());
    STATIC_REQUIRE(is_immutable_argument<const float[]>());
    STATIC_REQUIRE(is_immutable_argument<const bool[]>());
    STATIC_REQUIRE(is_immutable_argument<const E[]>());
    STATIC_REQUIRE(is_immutable_argument<const EC[]>());
    STATIC_REQUIRE(is_immutable_argument<const C[]>());
    STATIC_REQUIRE(is_immutable_argument<const std::nullptr_t[]>());
    STATIC_REQUIRE(!is_immutable_argument<int[]>());
    STATIC_REQUIRE(!is_immutable_argument<float[]>());
    STATIC_REQUIRE(!is_immutable_argument<bool[]>());
    STATIC_REQUIRE(!is_immutable_argument<E[]>());
    STATIC_REQUIRE(!is_immutable_argument<EC[]>());
    STATIC_REQUIRE(!is_immutable_argument<C[]>());
    STATIC_REQUIRE(is_immutable_argument<std::nullptr_t[]>());

    // referenced by member object pointer
    STATIC_REQUIRE(is_immutable_argument<const int C::*>());
    STATIC_REQUIRE(!is_immutable_argument<int C::*>());

    // referenced bei chain of pointers
    STATIC_REQUIRE(is_immutable_argument<const int* const* const*>());
    STATIC_REQUIRE(is_immutable_argument<const int* const* const* const>());
    STATIC_REQUIRE(!is_immutable_argument<int* const* const*>());
    STATIC_REQUIRE(!is_immutable_argument<const int** const*>());
    STATIC_REQUIRE(!is_immutable_argument<const int* const**>());
    STATIC_REQUIRE(!is_immutable_argument<int***>());
    STATIC_REQUIRE(!is_immutable_argument<const int***>());
    STATIC_REQUIRE(is_immutable_argument<const void* const* const*>());
    STATIC_REQUIRE(is_immutable_argument<const void* const* const* const>());
    STATIC_REQUIRE(is_immutable_argument<void* const* const*>());
    STATIC_REQUIRE(!is_immutable_argument<const void** const*>());
    STATIC_REQUIRE(!is_immutable_argument<const void* const**>());
    STATIC_REQUIRE(!is_immutable_argument<void***>());
    STATIC_REQUIRE(!is_immutable_argument<const void***>());

    // 2-dimensional array of pointers, possibly as references
    STATIC_REQUIRE(is_immutable_argument<const int* const* const[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int* const*[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int**[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<int* const*[][2]>());

    STATIC_REQUIRE(is_immutable_argument<const int* const* const(&&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int* const*(&&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int**(&&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<int* const*(&&)[][2]>());

    STATIC_REQUIRE(is_immutable_argument<const int* const* const(&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int* const*(&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int** const(&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<int* const* const(&)[][2]>());

    // additionally it is now a pointer chain pointing to the 2-dimensional array
    STATIC_REQUIRE(is_immutable_argument<const int* const* const(*const*)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int* const* const(**)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int* const*(*const*)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int** const(*const*)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<int* const* const(*const*)[][2]>());

    STATIC_REQUIRE(is_immutable_argument<const int* const* const(*const* const&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int* const* const(*const*&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int* const* const(**&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int* const*(*const* const&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int** const(*const* const&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<int* const* const(*const* const&)[][2]>());

    STATIC_REQUIRE(is_immutable_argument<const int* const* const(*const*&&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int* const*(*const*&&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int* const* const(**&&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<const int**(*const*&&)[][2]>());
    STATIC_REQUIRE(!is_immutable_argument<int* const*(*const*&&)[][2]>());
}
// NOLINTEND(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
