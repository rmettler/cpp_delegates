# C++ delegates

An extended solution for C++ delegates with better usability and full constexpr support, built from scratch using C++14 features.

It is inspired by the approach Sergey Ryazanov describes in the article [_The Impossibly Fast C++ Delegates_][impossDelegates]. He explains how you can use the fact that the address of any function or any member method of a class can be passed as non-type template argument and how this improves compile-time optimizations.

## Features

- Can store the address of a function or object and address of a member method of a given signature.
- Visibly documents this signature.
- Delegates calls to operator() to the stored function or member method.
  - Does a zero overhead call to an empty function, if the return type is void and the delegate was not set.  
    (Has undefined behavior if the return type is not void and the delegate was not set.)
- Provides factory functions for easy creation of such delegates.
- Goes without heap allocations and has full constexpr support. Hence, depending on your application, it can be fully compile time optimized and is a lot more performant than std::function, function-pointers or solutions based on virtual functions.

## Contents

Defined in [include/rome/delegates.hpp](include/rome/delegates.hpp):

- **delegate**: The original delegate.
  - If the return type is void, uses a safe implementation which calls an empty function if nothing or nullptr was assigned.
- **make_delegate**: A factory function which supports creating and assigning delegates.
- **event_delegate**: A restricted version of *delegate* which follows an event-driven forward only approach.
  - Restricts the return type to void.
  - Forbids function signatures with write access to the arguments.
- **make_event_delegate**: A factory function which supports creating and assigning event delegates.

## Examples

*TODO*

## Requirements

- A C++14 compatible compiler.


## Other solutions for C++ delegates

*TODO*

## References

1. Ryazanov, Sergey. “The Impossibly Fast C++ Delegates.” [_The Impossibly Fast C++ Delegates_][impossDelegates], CodeProject, 18 July 2005, www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates. 

[impossDelegates]: https://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates