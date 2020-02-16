# C++ delegates

- [Introduction](#introduction)
- [Library content](#library-content)
  - [`rome::delegate`](#romedelegate)
  - [`rome::fwd_delegate`](#romefwddelegate)
  - [`rome::event_delegate`](#romeeventdelegate)
  - [`rome::command_delegate`](#romecommanddelegate)
- [Examples](#examples)
  - [Default behavior of `rome::delegate`](#default-behavior-of-romedelegate)
  - [Usage of `rome::fwd_delegate`, `rome::command_delegate` and `rome::event_delegate`](#usage-of-romefwddelegate-romecommanddelegate-and-romeeventdelegate)
- [Integration](#integration)
- [Dependencies](#dependencies)
- [Tests](#tests)
- [Motivation](#motivation)
- [Comparision with other solutions](#comparision-with-other-solutions)
- [Thanks](#thanks)
- [References](#references)

## Introduction

The _C++ delegates_ library provides lightweight function wrappers able to store and invoke any callable target without the caller having to know the details of the callee. Examples of such callable targets are functions, member functions, function objects or lambda expressions. They just need to support the return and argument types specified with the wrapper.

This library is written in C++14 following the design goals below:

1. Safe to use:
   - No undefined behavior, no matter how the delegates are used.
   - Use compile-time checks instead of runtime-checks whenever possible.
2. Efficiency:
   - Ready to be used in memory and performance restricted software, e.g. on microcontrollers.
3. It shall be easy to understand, what the caller expects from the callee.
4. The delegates should still be easy to use.
5. Configurable behavior for calls when no target was assigned.

The original idea was taken from Sergey Ryazanov's [“The Impossibly Fast C++ Delegates”][impossDelegates], who describes an efficient function delegate mechanism. Yet, this library was written from scratch following the design goals mentioned above.

The behavior and interfaces of the _C++ delegates_ are similar to `std::function`. However, they trade their more efficient design and better performance with more verbose code when assigning a new target.

> A detailed comparision with `std::function`, other C++ callback mechanisms and other delegate implementations will follow.

## Library content

### `rome::delegate`

The basic delegate which supports any callable target of any function signature. On call, calls a previously assigned target. By default throws an exception when called without assigned target. But this behavior is configurable with the following options:

- `rome::target_is_expected`:  
  If no target was assigned before the `rome::delegate` is called, an exception is thrown. _(default)_
- `rome::target_is_optional`:  
  If no target was assigned before the `rome::delegate` is called, it directly returns without doing anything (only works if `Ret` == `void`).
- `rome::target_is_mandatory`  
  Ensures by design that there is always a target assigned to `rome::delegate`. For this the default constructor is deleted and the possibility to drop an assigned target is removed.

_See also the detailed documentation of [`rome::delegate`](doc/delegate.md) in [doc/delegate.md](doc/delegate.md)._

```cpp
namespace rome {
    template<typename Signature, typename Behavior = target_is_expected>
    class delegate; // undefined

    template<typename Ret, typename... Args, typename Behavior>
    class delegate<Ret(Args...), Behavior>;
}
```

### `rome::fwd_delegate`

Provides the same functionality as `rome::delegate`, but with the restriction that data can only be ___forwarded___. To ensure this, only function signatures with `void` return and arguments of immutable type are supported. E.g. the signature `void(const std::string&)` would work, while `void(int*)` or `bool()` would produce a compile error.

 _See also the detailed documentation of [`rome::fwd_delegate`](doc/fwd_delegate.md) in [doc/fwd_delegate.md](doc/fwd_delegate.md)._

```cpp
namespace rome {
    template<typename Signature, typename Behavior = target_is_expected>
    class fwd_delegate; // undefined

    template<typename... Args, typename Behavior>
    class fwd_delegate<void(Args...), Behavior>;
}
```

### `rome::event_delegate`

A [`rome::fwd_delegate`](doc/fwd_delegate.md) that ignores calls if no target was assigned.

This delegate was designed for event oder message-driven architectures, to notify about happened events. Thus, it is optional whether someone wants to listen to the event or not.

_See also the detailed documentation of [`rome::event_delegate`](doc/fwd_delegate.md) in [doc/fwd_delegate.md](doc/fwd_delegate.md)._

```cpp
namespace rome {
    template<typename Signature>
    using event_delegate = fwd_delegate<Signature, target_is_optional>;
}
```

### `rome::command_delegate`

A [`rome::fwd_delegate`](doc/fwd_delegate.md) that ensures that always a target is assigned.

This delegate was designed for event oder message-driven architectures to command an action that shall happen. Because the execution of the command is mandatory, a target must be assigned during construction of the delegate and can only be overriden by another target afterwards.

_See also the detailed documentation of [`rome::command_delegate`](doc/fwd_delegate.md) in [doc/fwd_delegate.md](doc/fwd_delegate.md)._

```cpp
namespace rome {
    template<typename Signature>
    using command_delegate = fwd_delegate<Signature, target_is_mandatory>;
}
```

## Examples

### Default behavior of `rome::delegate`

Define the delegate:

```cpp
rome::delegate<bool(int)> dgt;
// is equal to:
//  rome::delegate<bool(int), rome::target_is_expected> dgt;
```

When the delegate is called, an exception is thrown, since no target was assigned.

```cpp
try {
    bool b = dgt(42);
}
catch (const rome::bad_delegate_call& ex) {
    std::cout << ex.what() << '\n';
}
```

Console output:

> rome::bad_delegate_call

Assign a function with a matching signature to the delegate:

```cpp
bool isPositive(int value) {
    return value > 0;
}

dgt = decltype(dgt)::create<&isPositive>();
```

The delegate now calls the assigned function.

```cpp
std::cout << std::boolalpha << dgt(42) << '\n';
```

Console output:

> true

### Usage of `rome::fwd_delegate`, `rome::command_delegate` and `rome::event_delegate`

```cpp
struct CommandProcessor {
    rome::fwd_delegate<void(const std::vector<int>&)> onSubtractCommandRead; // (1)
    // is equal to:
    //  rome::fwd_delegate<void(const std::vector<int>&), rome::target_is_expected> onSubtractCommandRead;
    rome::command_delegate<void(const std::vector<int>&)> onAddCommandRead;  // (2)
    rome::event_delegate<void(const std::string&)> onParseError;             // (3)

    void processCommand(const std::string& line) const {
        std::istringstream iss{line.substr(2)};
        const std::vector<int> args{std::istream_iterator<int>{iss}, std::istream_iterator<int>{}};
        if ('+' == line.at(0)) {
            onAddCommandRead(args);
        }
        else if ('-' == line.at(0)) {
            onSubtractCommandRead(args);
        }
        else {
            onParseError(line);
        }
    }
    CommandProcessor(decltype(onAddCommandRead)&& dgt)
        : onAddCommandRead{std::move(dgt)} {
    }
};

int main() {
    const CommandProcessor cp{decltype(cp.onAddCommandRead)::create([](const std::vector<int>& args) {
        const auto result = std::accumulate(args.begin(), args.end(), 0);
        std::cout << "sum = " << result << '\n';
    })};
    const std::string cmd1{"+ 1 2 3"};
    const std::string cmd2{"- 1 2 3"};
    const std::string cmd3{"error"};
    std::cout << "cmd1:" << '\n';
    cp.processCommand(cmd1);
    // Calls the target assigned to the command_delegate (2). It was mandatory to assign a target to
    // this delegate and would have led to a compiler error otherwise.
    try {
        std::cout << "cmd2:" << '\n';
        cp.processCommand(cmd2);
        // Calls the fwd_delegate (1) with no assigned target. For this delegate, however, it is
        // expected that a target is assigned before call. -> an exception is thrown
    }
    catch (const rome::bad_delegate_call& ex) {
        std::cout << ex.what() << '\n';
    }
    std::cout << "cmd3:" << '\n';
    cp.processCommand(cmd3);
    // Calls the event_delegate (3) with no assigned target. For this it is optional that a target
    // is assigned before call. -> nothing happens
}
```

Console output:

> cmd1:  
> sum = 6  
> cmd2:
> rome::bad_delegate_call  
> cmd3:

## Integration

- Include the delegates in your code with:
  
  ```cpp
  #include <rome/delegate.hpp>
  ```

- Build your code for C++14 or later with the folder `include` added as a search path.

## Dependencies

The delegates only depend on following headers of the C++ standard library:

- `<algorithm>`
- `<cstddef>`
- `<exception>`
- `<type_traits>`
- `<utility>`

## Tests

The tests are found in [./test](./test).

The delegates are tested with following compilers on Ubuntu 18.04:

- clang++ 7.0.0
- g++ 7.4.0

The test builds are free of warnings, tested with `-Wall -Werror -Wextra -pedantic`.

Something above 34k runtime assertions are executed plus some static_asserts.

The single header test framework [doctest][doctest] version 2.3.5 is used for testing. Its header file is provided with the tests of the _C++ delegates_.

The tests can be executed by calling:

```bash
make test
```

## Motivation

I was searching for some kind of callback mechanism as unintrusive as invoking a function `foo()`. The function `foo()` doesn't specify from which language construct it needs be called and what the name of this construct needs to be. Whether I call it from a free function `bar()` or from a private member function `baz()` really doesn't matter. What matters are the name, the return type and the argument types the function `foo()`.

The same I am expecting for the opposite direction. A class specifies, that it wants to call something of specified return and argument types. But the class doesn't care what language construct is used to implement this call or how the callee would be named.

The most common solutions I usually see for callbacks are OOP interfaces and function pointers. Both are very intrusive and were inacceptable in my case. `std::function` would have worked, but isn't very performant.

During my investigations I stumbled over the very interesting article ["The Impssibly Fast C++ Delegates"][impossDelegates] by Sergey Ryazanov. But his delegates lead to undefined behavior, if called when no target is assigned. Other more modern implementations have this problem too.

This was a good motivation to start my own delegate implementation.

## Comparision with other solutions

> A comparision with `std::function`, function pointers, OOP interfaces, static polymorphism using template arguments and other delegate implementations will follow.
>
> E.g.:  
> It is always defined behavior to call one of the _C++ delegates_, even so no target was assigned. Moreover, the behavior of the call to an empty delegate can be configured using an optional template parameter. This is an advantage to `std::function` and other delegate implemantions.

## Thanks

- Sergey Ryazanov for his incredible article [_The Impossibly Fast C++ Delegates_][impossDelegates]. He explains how you can use the fact that the address of any function can be passed as non-type template argument to create highly optimizable function delegates.
- Lee David for help finding more comprehensible names
- Matthias Deimbacher for setting up the CI pipelines at gitlab

## References

1. Ryazanov, Sergey. “The Impossibly Fast C++ Delegates.” [_The Impossibly Fast C++ Delegates_][impossDelegates], CodeProject, 18 July 2005, www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates.
2. Cppreference. [_std::function_][std_function], cppreference.com, 17 September 2019, https://en.cppreference.com/w/cpp/utility/functional/function, CC-BY-SA.
3. Kirilov, Viktor. [_doctest_][doctest], 05.02.2020, https://github.com/onqtam/doctest, MIT

[impossDelegates]: https://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
[std_function]: https://en.cppreference.com/w/cpp/utility/functional/function
[doctest]:  https://github.com/onqtam/doctest
