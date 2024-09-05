# _rome::_ **delegate**

Defined in header [`<rome/delegate.hpp>`](../include/rome/delegate.hpp).

```cpp
template<typename Signature, typename Behavior = target_is_expected>
class delegate;  // undefined

template<typename Ret, typename... Args, typename Behavior>
class delegate<Ret(Args...), Behavior>;
```

Instances of class template `rome::delegate` can store and invoke any callable _target_ -- functions, lambda expressions, std::function, other function objects, as well as static and non-static member functions.

The stored callable object is called the **_target_** of `rome::delegate`. If a `rome::delegate` contains no _target_, it is called **_empty_**.

Invoking the _target_ of an _empty_ `rome::delegate` results in a defined behavior configurable by the `Behavior` template parameter (see [_Template parameters_](#template-parameters) below).

Assigning a function object _target_ to a `rome::delegate` can be done during construction of or with an assignment to the `rome::delegate`. Other callable _targets_ such as functions and member functions either need to be wrapped by a function object (e.g. by a lambda expression) or use the provided factory method [`create`](delegate/create.md). See also the [_Examples_](#examples) below.

Small _targets_ are stored in the local storage of a `rome::delegate`. Such small object optimization takes place if:

- the _target_ is a function object of size smaller or equal to `sizeof(void*)`  
  _e.g. a lambda expression capturing a pointer_
- the _target_ is a function
- the _target_ is a member function, both the member function pointer and the reference to the object are stored locally

If a function object _target_ is bigger than `sizeof(void*)`, new storage is dynamically allocated.

The size of a `rome::delegate` is the size of an object pointer plus twice the size of a function pointer:

```cpp
sizeof(rome::delegate<Ret(Args...), Behavior>)
    == sizeof(void*) + 2*sizeof(void (*)())
```

A `rome::delegate` is moveable but not copyable.

## Template parameters

- `Ret`  
  The return type of the _target_ being called.
- `Args...`  
  The argument types of the _target_ being called.
- `Behavior`  
  Defines the behavior of an _empty_ `rome::delegate` being called. Defaults to `rome::target_is_expected`.
  
  The behavior can be chosen by declaring the delegate with one the following types:
  
  - `rome::target_is_expected`  
    A valid _target_ is expected to be assigned before the `rome::delegate` is called.  
    When an _empty_ `rome::delegate` is being called:
    - Throws a [`rome::bad_delegate_call`](./bad_delegate_call.md) exception.
    - Instead calls [`std::terminate`](https://en.cppreference.com/w/cpp/error/terminate), if exceptions are disabled.
  - `rome::target_is_optional` _(only if `Ret`==`void`)_  
    Assigning a _target_ to the `rome::delegate` is optional. Calling an _empty_ delegate returns directly without doing anything.  
    Compile error, if `Ret` != `void`.
  - `rome::target_is_mandatory`  
    Prevents by design that a `rome::delegate` can be _empty_. This has following consequences:

    - Default constructor is deleted. A new instance of `rome::delegate` can only be created by passing a _target_ to the [constructor](delegate/constructor.md) or by using one of the factory functions [create](delegate/create.md).
    - There is no possibility to drop a currently assigned _target_, though it can be overridden by assigning a new _target_.

    _Note: The `rome::delegate` still becomes_ empty _after a move, i.e., after `auto y = std::move(x)`_ x _is_ empty _and behaves as if `Behavior` was set to `rome::target_is_expected`._

## Member functions

- [constructor](delegate/constructor.md)  
  constructs a new `rome::delegate` instance
- [desctructor](delegate/destructor.md)  
  destroys a `rome::delegate` instance
- [operator=](delegate/operator_assignment.md)  
  assigns or drops a _target_
- [swap](delegate/swap.md)  
  swaps the _targets_
- [operator bool](delegate/operator_bool.md)  
  checks if a valid _target_ is contained
- [operator()](delegate/operator_function_call.md)  
  invokes the _target_
- [create](delegate/create.md) - _static_  
  creates a new `rome::delegate` instance with given _target_ assigned

## Non-member functions

- [rome::swap](delegate/swap2.md)  
  swaps the _targets_ of two `rome::delegate` instances
- [operator==, operator!=](delegate/operator_cmp_nullptr.md)  
  compares `rome::delegate` with nullptr

## Notes

The most similar C++ standard library counterpart is [std::move_only_function](https://en.cppreference.com/w/cpp/utility/functional/move_only_function) (since C++23). It has similar behavior and interface, with following main differences:

- undefined behavior when called empty
- specifiable cv-qualifiers, ref-qualifiers and noexcept-specifiers for `operator()`
- direct assignment of functions and member functions possible  
  _With `rome::delegate` they need to be wrapped by a function object (e.g. lambda expression) or by using the [create](delegate/create.md) function._
- unspecified storage size for small object optimization

## Examples

Basic usage examples for all three types of `Behavior` and the three target types function, member function and function object.

_See the code in [examples/basic_examples.cpp](../examples/basic_examples.cpp)._

```cpp
#include <functional>
#include <iostream>
#include <utility>
#include <rome/delegate.hpp>

void print(int i) {
    std::cout << i << '\n';
}

int plus100(int i) {
    return i + 100;
}

struct TargetClass {
    int value = 0;

    void set(int i) {
        value = i;
    }
};

struct Example {
    rome::delegate<int(int), rome::target_is_mandatory> onMandatoryNotEmpty;
    rome::delegate<int(int) /*, rome::target_is_expected*/> onExpectedNotEmpty;  // (1)
    // rome::delegate<int(int), rome::target_is_optional> onMaybeEmpty;          // (2) does not compile
    rome::delegate<void(int), rome::target_is_optional> onMaybeEmpty;

    Example(decltype(onMandatoryNotEmpty)&& mand) : onMandatoryNotEmpty{std::move(mand)} {  // (3)
    }
};

int main() {
    TargetClass obj{};
    Example x{std::negate<>{}};                       // (3)

    std::cout << "Calls after initialization:\n";
    print(x.onMandatoryNotEmpty(1));
    try {
        x.onExpectedNotEmpty(2);
    }
    catch (const rome::bad_delegate_call& e) {
        std::cout << e.what() << '\n';
    }
    x.onMaybeEmpty(3);

    std::cout << "\nCalls with fresh assigned targets:\n";
    x.onMandatoryNotEmpty = [](int i) { return i + 10; };
    x.onExpectedNotEmpty  = [](int i) { return plus100(i); };
    x.onMaybeEmpty        = [&obj](int i) { obj.set(i); };
    print(x.onMandatoryNotEmpty(4));
    print(x.onExpectedNotEmpty(5));
    print(obj.value);
    x.onMaybeEmpty(6);
    print(obj.value);

    std::cout << "\nCalls after dropping targets:\n";
    // x.onMandatoryNotEmpty = nullptr;               // (4) does not compile
    x.onExpectedNotEmpty  = nullptr;
    x.onMaybeEmpty        = nullptr;
    print(x.onMandatoryNotEmpty(7));
    try {
        x.onExpectedNotEmpty(8);
    }
    catch (const rome::bad_delegate_call& e) {
        std::cout << e.what() << '\n';
    }
    x.onMaybeEmpty(9);
    print(obj.value);
}
```

- **(1)** - second template parameter is `rome::target_is_expected` by default
- **(2)** - `rome::delegate` with `rome::target_is_optional` must have void return
- **(3)** -  `rome::delegate` with `rome::target_is_mandatory` has deleted default constructor, a _target_ must be assigned during construction
- **(4)** - `rome::delegate` with `rome::target_is_mandatory` does not allow to drop _targets_

Output:

> Calls after initialization:  
> -1  
> rome::bad_delegate_call  
>
> Calls with fresh assigned targets:  
> 14  
> 105  
> 0  
> 6  
>
> Calls after dropping targets:  
> 17  
> rome::bad_delegate_call  
> 6

## See also

- [rome::fwd_delegate](fwd_delegate.md)  
  The same as `rome::delegate` but restricts data to be forwarded only.
- [std::move_only_function](https://en.cppreference.com/w/cpp/utility/functional/move_only_function) (C++23)  
  Wraps a callable object of any type with specified function call signature.
- [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) (C++11)  
  Wraps a callable object of any copy constructible type with specified function call signature.
