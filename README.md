# C++ delegates

***Here are some more advanced C++ delegates under development***

A lightweight function interface which may store and call any callable compatible with the specified function signature. It provides higher flexibility with less effort than function pointers, function objects or OOP interfaces. It is similar to use, but more performant than `std::function`. Compared to other delegate solutions it prevents any undefined behavior.

## Content under development

[`rome::delegate`][rome_delegate_md]:

- can be used for any callble
- supports all function signatures
- by default requires a target being assigned, otherwise throws an exception
- if exceptions are disabled, calls std::terminate to prevent undefined behavior
- to be released soon

`rome::event_delegate`:

- intended to be used in event driven communication, e.g. for event based components
- restricts the function signature to:
  - Only `void` as return type.
  - Only values, r-value references or any const reference or pointer type as pointer typse.
    Thus prevents the callee could change the state of the caller.
- By default assigning a target to the delegate is optional. If nothing is assigned, an empty function is called.

## Documentation

Until now only [`rome::delegate`][rome_delegate_md] is documented. There are still a lot of examples and some references missing. And since it is not yet fully implemented and tested, the interfaces may change.

See [`rome::delegate`][rome_delegate_md]

## Goal

I was searching for a solution where I could understandably specify events that happen in a module and listen to them, if I want to. Then I want to be able to just assign any callable that matches the signature of the event raised and do this by just linking them together. And this solution should be lightweight, safe and be usable in a modern embedded environment.

During my investigations I stumbled over the [Impssibly Fast C++ Delegates by Sergey Ryazanov] which inspired me. I then found many adaptations for modern C++, but I didn't find any which satisfied my needs. So I started my own project.

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
- functor/function object (compile time interface)
- signal/slot
- OOP interfaces
- Impossibly fast delegates
- Fastest possible delegates
- function_ref
- https://github.com/LoopPerfect/smallfunction, https://hackernoon.com/experimenting-with-small-buffer-optimization-for-c-lambdas-d5b703fb47e4

> **Comparision with `std::function`**
>
> - Similar declaration and usage as `std::function`.
> - Trades a bit of usability for a more efficient implementation than `std::function`.  
>   > (TODO: reference a benchmark that shows that this is true for both  the libstdc++ and libc++ (when std::function uses SBO and when not)!)
> - Already stores the object pointer for non-static member functions, so  no `std::bind` or additional lambda is needed.
> - `rome::delegate` can be configured to ignore a call to an _empty_ > `rome::delegate`, where `std::function` always throws an exception.
> - Size of `rome::delegate` is specified.  
> - Definable behavior when calling an empty `rome::delegate`, `std::function` always throws an exception.
>   > (TODO: do this in tabular form listing the features)

## Thanks

- The basic delegate is inspired by the approach Sergey Ryazanov describes in the article [_The Impossibly Fast C++ Delegates_][impossDelegates]. He explains how you can use the fact that the address of any function can be passed as non-type template argument to create highly optimizable function delegates.
- The documentation of the delegates used parts of the documentation of [_std::function_][std_function] at cppreference.com.
- Lee David helped finding more comprehensible names.

## References

1. Ryazanov, Sergey. “The Impossibly Fast C++ Delegates.” [_The Impossibly Fast C++ Delegates_][impossDelegates], CodeProject, 18 July 2005, www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates.
2. Cppreference. [_std::function_][std_function], cppreference.com, 17 September 2019, https://en.cppreference.com/w/cpp/utility/functional/function, CC-BY-SA.

[impossDelegates]: https://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
[rome_delegate_md]: doc/delegate.md
[std_function]: https://en.cppreference.com/w/cpp/utility/functional/function
