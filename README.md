# C++ delegates

A lightweight function interface providing higher flexibility with less effort than function pointers, function objects or OOP interfaces. Is similar to use, but more performant than `std::function`.

**UNDER CONSTRUCTION: The whole readme needs a lot more work.**

***

## Table of contents

- [C++ delegates](#C-delegates)
  - [Table of contents](#Table-of-contents)
  - [Goals](#Goals)
    - [When to use](#When-to-use)
    - [When not to use](#When-not-to-use)
  - [Features](#Features)
  - [Examples](#Examples)
    - [Event delegates](#Event-delegates)
  - [System requirements](#System-requirements)
  - [Other solutions and other implementations of C++ delegates](#Other-solutions-and-other-implementations-of-C-delegates)
  - [Thanks](#Thanks)
  - [References](#References)

## Goals

There are a lot of articles, examples and libraries out there regarding differnt kinds of C++ delegates. So why yet another one?

- **Function level interfaces**. TODO: should add abstraction, caller should not have to care about the callee, as long as the callees functions argument and return types match. changed during runtime.
- **Performance**. (TODO) as less overhead as somehow possible, in speed and code size. Usable for the embedded environment. can compete with any other runtime solution. uses only two pointers and no dynamic memory allocations.
- **Safe**. (TODO) No undefined behavior when using `void` as return type no callee is set. Omitting runtime checks and thus retaining maximal performance.
- **Easy to use**. (TODO) supply helpful error messages during compile time. can be copied and compared.

### When to use

- Performance and/or memory constrained environment.
- Function objects or other static interfaces are infeasible.

### When not to use

If performance is no issue then std::function may be a better fit. (TODO cite std::function)

If callers and callees are known during compile time and you are comfortable using templates, e.g. functors are safer and more performant, but in some occasions may produce more code. (TODO cite functors)

## Features

- Can store the address of a function or object and address of a member method of a given signature.
- Visibly documents this signature.
- Delegates calls to operator() to the stored function or member method.
  - Does a zero overhead call to an empty function, if the return type is void and the delegate was not set.  
    (Has undefined behavior if the return type is not void and the delegate was not set.)
- Provides factory functions for easy creation of such delegates.
- Goes without heap allocations and has full constexpr support. Hence, depending on your application, it can be fully compile time optimized and is a lot more performant than std::function, function-pointers or solutions based on virtual functions.

Defined in [include/rome/delegates.hpp](include/rome/delegates.hpp):

- **delegate**: The original delegate.
  - If the return type is void, uses a safe implementation which calls an empty function if nothing or nullptr was assigned.
- **make_delegate**: A factory function which supports creating and assigning delegates.
- **event_delegate**: A restricted version of *delegate* which follows an event-driven forward only approach.
  - Restricts the return type to void.
  - Forbids function signatures with write access to the arguments.
- **make_event_delegate**: A factory function which supports creating and assigning event delegates.

## Examples

### Event delegates

Describe an interface with an `event_delegate`:

```cpp
using delegates = rome::delegates;
delegates::event_delegate<void(int)> valueChanged;
```

When the delegate is called, nothing happens, since the delegate is not set.

```cpp
valueChanged(42);
// nothing happens
```

Link a function with a matching signature to the delegate:

```cpp
void print(int value) {
    std::cout << "value = " << value << std::endl;
}

valueChanged = delegates::make_event_delegate<
        decltype(&print), &print>();
```

The delegate now calls the linked function.

```cpp
valueChanged(42);
// Output: value = 42
```

## System requirements

- A C++14 compatible compiler and standard library.
- Has no other dependencies.

## Other solutions and other implementations of C++ delegates

> TODO

- function pointer
- std::function
- functor/function object (static interface)
- signal/slot
- OOP interfaces
- Impossibly fast delegates
- Fastest possible delegates
- function_ref
- https://github.com/LoopPerfect/smallfunction, https://hackernoon.com/experimenting-with-small-buffer-optimization-for-c-lambdas-d5b703fb47e4

## Thanks

The basic delegate inspired by the approach Sergey Ryazanov describes in the article [_The Impossibly Fast C++ Delegates_][impossDelegates]. He explains how you can use the fact that the address of any function can be passed as non-type template argument to create highly optimizable function delegates.

## References

1. Ryazanov, Sergey. “The Impossibly Fast C++ Delegates.” [_The Impossibly Fast C++ Delegates_][impossDelegates], CodeProject, 18 July 2005, www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates.

[impossDelegates]: https://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates