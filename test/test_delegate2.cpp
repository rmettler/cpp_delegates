//
// Project: C++ delegates
//
// Copyright Roger Mettler 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//

#include <doctest.h>

#include <cassert>
#include <tuple>
#include <type_traits>

#include "rome/delegates.hpp"

// =============================================================================
// Explicit template instantiations
// =============================================================================
template class ::rome::delegates::delegate<void()>;

// =============================================================================
// Test cases
// =============================================================================

// clang-format off

// TODO: overhaul all the namings and functions! -> see std::function
// - parameter: provided by the function
// - argument: passed to the function
// - the _delegate_ is a "function wrapper" (able to wrap functions at compile-time or run-time)
// - instances of _delegate_ can store, copy and invoke any _Callable_ target??? (really?) -> yes for functions, member functions, function objects. But also for data members, lambda expressions and bind expressions?
//   care: ensure that function objects or objects used for member functions are not moved to it, since they are only referenced and the user has to ensure himself, that those objects remain valid during the lifetime of the delegate
// - the stored callable object is called the _target_ of _delegate_
// - if the _delegate_ contains no _target_, it is called _empty_
// - _delegate_ satisfies the requirements of _CopyConstructible_, _CopyAssigneable_ and _EqualityComparable (and maybe more?)
//   TODO: base the tests on these name requirements!
// TODO: delegate(std::nullptr_t = nullptr) : members{} { }
// TODO: remove all usings from event_delegate
// TODO: friend bool operator==(l,r)
// TODO: explicit operator bool()
// TODO: -isLinked/isUnlinked

// In documentation:


// Requirements:
// Provide a mechanism to sequentially delegate a function call to another function.
// TODO: User stories in schönere prose umformulieren und nachher requirements ableiten (nummerieren)
// TODO: In text "delegate" bereits einbauen (to delegate a function call, delegate a decision)
// TODO: possible solutions: function pointer, functor, interface (virtual method), std::function, function_ref/function_view, delegate
// What do we want to accomplish? (do it with examples)
//  User story 1:
//      As a developer of a software module,
//          I want to specify a function interface consisting of argument types and a return type,
//          so that the user of the module can provide the function the module needs.
//      Example:
//          The module needs the user of the module to select an item from a list of strings. For this, the module
//          calls a function with the signature `int(const std::vector<std::string>&)`.
//          The user wants following member function to be called by the module:
//          ```Cpp
//          struct SomeClass {
//              std::string selection_;
//              int select(const std::vector<std::string>& items) {
//                  auto found = std::find(items.begin(), items.end(), selection_);
//                  return (found != itms.end()) ? found-items.begin() : -1;
//              }
//          };
//          ```
//          Question: How can SomeClass::select efficiently be assigned to the module for being called.
//  
//  User story 2:
//      As a developer of a software module,
//          I want to specify a function interface consisting of argument types and a return type,
//          so that the user of the module may assign a callback function to receive state change information
//          and I don't have to care whether the user has assigned a function or not.
//      Example:
//          We have a software module in which a state change happens (1) and the module informing about the changed value (2).
//          struct SomeModule {
//              int value_;
//              void recalculateValue() {
//                  value_ = someFancyCalculation();  // (1) calculare new state
//                  valueChanged(value_);             // (2) notify the state change
//              }
//          };
//          SomeModule module1;  // (3)
//          SomeModule module2;  // (4)
//          `module1` (1) is used in a way, that every state change should be printed with the following function:
//          void printValue(int value) {
//              std::cout << value << std::endl;
//          }
//          With `module2` (4) the state change is of no interrest, thus, no function is provided for the valueChanged notification (2).
//          Question: How can in one case an internal function call efficently be delegated to a user function and in another case the internal function call is ignored because no delegated function is applied?
//
//  User story 3:
//      As the user of a software module,
//          
//  
// non-functional:
//  1. performance
//      The target function shall be 
//  2. safety
//  3. usability

// Think about possible solutions -> make an article

// Specs:
// rome::delegates::delegate<T>:
// A template class.
// Template Arguments: T


// The library provides a template class of name "delegate".
// 1. Declaration
    // 1. "delegate" must take exactly one template argument.
    // 2. The template argument taken must be a function signature of type Ret(Args...) with Ret the return type and Args... the list of the function's argument types. As argument types are all types allowed also allowed for function arguments.
    // 3. A compile error must be issued if an object of delegate is created and:
    //    a. delegate was declared with an invalid number of template arguments, as required in (1).
    //    b. delegate was declared with a template argument wich is not of the type of a function signature as required in (2).
    //    In both cases the compile error message shall show the template arguments used, a message that the template arguments are invalid and an example how to declare delegate correctly.
// 2a. sizeof(delegate) <= 2*sizeof(void*) // since void* is capable to hold any pointer and we need to store an object and a function pointer
// 2. An object of delegate must satisfy following named requirements as used in the normative text of the C++ standard to define the expectations of the standard library:
//    DefaultConstructible, MoveConstructible, CopyConstructible, MoveAssignable, CopyAssignable, Destructible
//    TriviallyCopyable, StandardLayoutType
//    EqualityComparable, Swappable, NullablePointer, FunctionObject, Callable, NumericType
// std::is_nothrow_default_constructible<...>::value
// std::is_nothrow_copy_constructible<...>::value
// std::is_nothrow_move_constructible<...>::value
// std::is_copy_assignable<...>::value
// std::is_move_assignable<...>::value
// 3. If the delegate is not assigned to a target, it is called EMPTY.
//    a. An EMPTY delegate must have following behavior:
//       - (d == nullptr) == true, (nullptr == d) == true
//       - (d != nullptr) == false, (nullptr != d) == false
//       - !d == true, !!d == false
//       - compares equal with another empty delegate.
//       - d(args...) does nothing, if the return type is void.
//       - undefined behavior, if the return type is not void.
//    a. A delegate with an assigned target is therefore not EMPTY and must have following behavior:
//       - (d == nullptr) == false, (nullptr == d) == false
//       - (d != nullptr) == true, (nullptr != d) == true
//       - !d == false, !!d == true
//       - does not compare equal with an empty delegate.
//       - d(args...) calls the target function.
// 4. Construction
//    a. delegate is DefaultConstructible. BDD description:
//       GIVEN a default cunstructed delegate,
//           THEN the delegate is EMPTY.
//       GIVEN a default constructed delegate AND another EMPTY delegate,
//           THEN both compare equal.
//    b.


    // clang-format on
    TEST_CASE("Testing delegate<void()>") {
    using rome::delegates::delegate;
    using rome::delegates::make_delegate;

    using TDelegate = delegate<void()>;

    SUBCASE("The delegate has a valid signature.") {
        CHECK(!asserts_signature<TDelegate>);
    }

    GIVEN("a default constructed delegate") {
        delegate<void()> dgt;

        THEN("no callee shall be linked and a comparision with nullptr shall "
             "return equal") {
            checkUnlinked(dgt);
        }
        WHEN("it is compared with another default constructed delegate") {
            delegate<void()> other;
            THEN("it shall compare equal") {
                CHECK((dgt == other) == true);
                CHECK((dgt != other) == false);
            }
        }
        WHEN("it is compared with a nullptr initialized delegate") {
            delegate<void()> dNull(nullptr);
            THEN("it shall compare equal") {
                CHECK((dgt == dNull) == true);
                CHECK((dgt != dNull) == false);
            }
        }
        WHEN("it is compared with a delegate with a linked callee") {
            delegate<void()> dLinked = make_delegate<decltype(&function), &function>();
            THEN("it shall compare unequal") {
                CHECK((dgt == dLinked) == false);
                CHECK((dgt != dLinked) == true);
            }
        }
        WHEN("operator() is called") {
            callCounter = 0;
            dgt();
            THEN("nothing happened") {
                CHECK(callCounter == 0);
            }
        }
        WHEN("nullptr is assigned") {
            dgt = nullptr;
            THEN("it is still unlinked") {
                checkUnlinked(dgt);
            }
        }
        // TODO: do everything from here
        WHEN("another delegate is copy-assigned") {
            delegate<void()> src1 = make_delegate<decltype(&function), &function>();
            THEN("it takes the behavior of the assigned delegate") {
                // TODO test it
            }
            delegate<void()> src2 = make_delegate<decltype(&function2), &function2>();
            ;
            WHEN("another delegate is copy-assigned again") {
            }
            // and compared with other delegates (changes in function and
            // object) and compared with default constructed delegate and
            // operator() is called nullptr is assigned again and compared with
            // default constructed delegate operator() is called
        }
        WHEN("another delegate is move-assigned") {
            // and operator() is called
            // nullptr is assigned again
            // operator() is called}
        }
        GIVEN("a nullptr initialized delegate") {
        }
        GIVEN("a copy-constructed delegate") {
        }
        GIVEN("a move-constructed delegate") {
        }
        GIVEN("a factory created delegate") {
        }
    }
}
