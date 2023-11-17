# C++ delegates

- [Introduction](#introduction)
- [Library content](#library-content)
  - [`rome::delegate`](#romedelegate)
  - [`rome::fwd_delegate`](#romefwd_delegate)
  - [`rome::event_delegate`](#romeevent_delegate)
  - [`rome::command_delegate`](#romecommand_delegate)
- [Integration](#integration)
- [Documentation](#documentation)
- [Examples](#examples)
  - [Usage of `rome::delegate`](#usage-of-romedelegate)
  - [Usage of `rome::command_delegate` and `rome::event_delegate`](#usage-of-romecommand_delegate-and-romeevent_delegate)
- [Dependencies](#dependencies)
- [Comparision with other solutions](#comparision-with-other-solutions)
- [Design decisions](#design-decisions)
- [Tests](#tests)
- [Feedback](#feedback)
- [Motivation](#motivation)
- [Thanks](#thanks)
- [References](#references)

## Introduction

The _C++ delegates_ library provides lightweight function wrappers able to store and invoke any callable target without the caller having to know the details of the callee. Examples of such callable targets are functions, member functions, function objects or lambda expressions. They just need to support the function call signature specified with the delegate.

This library is written in C++14 following the design goals below:

1. Safe to use:
   - No undefined behavior, no matter how the delegates are used.
   - Use compile-time checks instead of runtime-checks whenever possible.
2. Efficiency:
   - Ready to be used in memory and performance restricted software, e.g. on microcontrollers.
3. Easy to understand, what the caller expects from the callee.
4. Easy to use.
5. Configurable behavior for calls when no target was assigned.

## Library content

### `rome::delegate`

The basic delegate which supports any callable target of any function signature. On call, calls a previously assigned target. By default throws an exception when called without assigned target. But this behavior is configurable with the following options:

- `rome::target_is_expected`:  
  If no target was assigned before the `rome::delegate` is called, an exception is thrown. _(default)_
- `rome::target_is_optional`:  
  If no target was assigned before the `rome::delegate` is called, it directly returns without doing anything (only works if `Ret` == `void`).
- `rome::target_is_mandatory`  
  Ensures by design that there is always a target assigned to `rome::delegate`. For this, the default constructor is deleted and there is no possibility to drop an assigned target.

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

Provides the same functionality as `rome::delegate`, but with the restriction that data can only be **_forwarded_**. To ensure this, only function signatures with `void` return and arguments of immutable type are supported. E.g. the signature `void(const std::string&)` would work, while `void(int*)` or `bool()` would produce a compile error.

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

Designed for event oder message-driven architectures, to notify about happened events. Thus, it is optional whether someone wants to listen to the event or not.

_See also the detailed documentation of [`rome::event_delegate`](doc/fwd_delegate.md) in [doc/fwd_delegate.md](doc/fwd_delegate.md)._

```cpp
namespace rome {
    template<typename Signature>
    using event_delegate = fwd_delegate<Signature, target_is_optional>;
}
```

### `rome::command_delegate`

A [`rome::fwd_delegate`](doc/fwd_delegate.md) that ensures that always a target is assigned.

Designed for event oder message-driven architectures to command an action that shall happen. Because the execution of the command is mandatory, a target must be assigned during construction of the delegate and can only be overriden by another target afterwards.

_See also the detailed documentation of [`rome::command_delegate`](doc/fwd_delegate.md) in [doc/fwd_delegate.md](doc/fwd_delegate.md)._

```cpp
namespace rome {
    template<typename Signature>
    using command_delegate = fwd_delegate<Signature, target_is_mandatory>;
}
```

## Integration

- Add the folder `./include` to the include paths of your compiler.
- Include the delegates in your code with:
  
  ```cpp
  #include <rome/delegate.hpp>
  ```

Or simply copy the single header file in `./include/rome` to your project.

## Documentation

Please see the documentation in the folder `./doc`. Especially the following markdown files:

- [doc/delegate.md](doc/delegate.md)
- [doc/fwd_delegate.md](doc/fwd_delegate.md)

## Examples

### Usage of `rome::delegate`

Basic usage examples for all three types of `Behavior` and the three target types function, member function and function object.

_See the code in [examples/basic_examples.cpp](../examples/basic_examples.cpp)._

```cpp
#include <functional>
#include <iostream>
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
    Example x{std::negate<>{}};             // (3)

    std::cout << "Calls after initialization:\n";
    print(x.onMandatoryNotEmpty(1));        // `std::negate<>` was assigned
    try {
        x.onExpectedNotEmpty(2);            // called empty
    }
    catch (const rome::bad_delegate_call& e) {
        std::cout << e.what() << '\n';
    }
    x.onMaybeEmpty(3);                      // called empty

    std::cout << "\nCalls with fresh assigned targets:\n";
    // assign function object
    x.onMandatoryNotEmpty = [](int i) { return i + 10; };
    // assign function object wrapping a function
    x.onExpectedNotEmpty  = [](int i) { return plus100(i); };
    // assign function object wrapping a member function
    x.onMaybeEmpty        = [&obj](int i) { obj.set(i); };
    print(x.onMandatoryNotEmpty(4));
    print(x.onExpectedNotEmpty(5));
    print(obj.value);
    x.onMaybeEmpty(6);
    print(obj.value);

    std::cout << "\nCalls after dropping targets:\n";
    // x.onMandatoryNotEmpty = nullptr;     // (4) does not compile
    x.onExpectedNotEmpty  = nullptr;
    x.onMaybeEmpty        = nullptr;
    print(x.onMandatoryNotEmpty(7));        // function object still assigned
    try {
        x.onExpectedNotEmpty(8);            // called empty
    }
    catch (const rome::bad_delegate_call& e) {
        std::cout << e.what() << '\n';
    }
    x.onMaybeEmpty(9);                      // called empty
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

### Usage of `rome::command_delegate` and `rome::event_delegate`

Model of an extremely simplified cruise control system. The four classes _Engine_, _BrakingSystem_, _SpeedSensor_ and _CruiseControl_ are atomic, i.e., are free from dependencies to other classes. _Integration_ integrates all four.

_See the code in [examples/cruise_control.cpp](../examples/cruise_control.cpp)._

```cpp
#include <iostream>
#include <rome/delegate.hpp>

struct Engine {
    void accelerate() {
        std::cout << "engine accelerating\n";
    }
    void turnOff() {
        std::cout << "engine turned off\n";
    }
};

struct BrakingSystem {
    void turnBrakesOn() {
        std::cout << "brakes on\n";
    }
    void turnBrakesOff() {
        std::cout << "brakes off\n";
    }
};

struct SpeedSensor {
    // Assigning delegate is optional for speed sensor to work.
    rome::event_delegate<void(float)> onSpeedChanged;
};

class CruiseControl {
    float targetSpeed_ = 0.0F;

    // Assigning both delegates is required for cruise control to work.
    rome::command_delegate<void()> onAccelerateCar_;
    rome::command_delegate<void()> onSlowDownCar_;

  public:
    void updateAcceleration(const float drivingSpeed) {
        if (drivingSpeed < targetSpeed_ * 0.95F) {
            onAccelerateCar_();
        }
        else if (drivingSpeed > targetSpeed_ * 1.05F) {
            onSlowDownCar_();
        }
    }

    void setTargetSpeed(const float targetSpeed) {
        targetSpeed_ = targetSpeed;
    }

    CruiseControl(rome::command_delegate<void()>&& onAccelerateCar,
        rome::command_delegate<void()>&& onSlowDownCar)
        : onAccelerateCar_{std::move(onAccelerateCar)}, onSlowDownCar_{std::move(onSlowDownCar)} {
    }
};

struct Integration {
    SpeedSensor speedSensor;
    CruiseControl cruiseControl;
    Engine engine;
    BrakingSystem brakes;

    Integration()
        : cruiseControl{
            [this]() {
                brakes.turnBrakesOff();
                engine.accelerate();
            },
            [this]() {
                engine.turnOff();
                brakes.turnBrakesOn();
            }} {
        speedSensor.onSpeedChanged = [this](float drivingSpeed) {
            cruiseControl.updateAcceleration(drivingSpeed);
        };
    }
};

Integration integration{};

int main() {
    // Simulate IO not connected in this example
    integration.cruiseControl.setTargetSpeed(25.0F);
    integration.speedSensor.onSpeedChanged(20.0F);
    integration.speedSensor.onSpeedChanged(25.0F);
    integration.speedSensor.onSpeedChanged(30.0F);
}
```

Output:

> brakes off  
> engine accelerating  
> engine turned off  
> brakes on

## Dependencies

The delegates only depend on following headers of the C++ standard library:

- `<algorithm>`
- `<cstddef>`
- `<exception>`
- `<type_traits>`
- `<utility>`

## Comparision with other solutions

Similar C++ standard library counterparts, in behavior and interface:

- `std::move_only_function` (C++23)  
  Very similar by constraints and efficency. However, it is undefined behavior when an `std::move_only_function` is called empty. For _C++ delegates_, this behavior is definable.
- `std::function` (C++11)  
  Copyable but less efficient. Throws an exception when called empty.

## Design decisions

- Why do delegates take ownership of assigned function objects?  
  Lets take the example `delegate<void()> d = []() {};`. If the delegate would only reference the function object created by the lambda expression, it may easily happen, that the assigned function object is already destroyed when the delegate is called. To make safe usage of the delegate easy and unsafe usage hard, the delegate takes the ownership of a function object.  
  Note, that it is still necessary to manage the lifetime of objects that the function object captures by reference.

- Why is the size of the delegate `sizeof(void*) + 2*sizof(void(*)())`?
  
  - One function pointer stores the function that invokes the target.
  - One function pointer stores the function that destroys an assigned function object. This happens either when the delegate is destroyed or when a target is dropped.
  - The object pointer stores the address where the function object is stored. Or, with small object optimization, the function object is stored within the memory of the pointer.
  
  Thus, it is already the minimum size needed to achieve the above. And the size is kept at that minimum for to be available for memory restricted devices.

- Why can't I copy delegates, why are they move-only?
  
  - It would need another function pointer stored within the delegate that increases its size significantly.
  - Copying the delegate may only lead to a shallow copy of the target but a deep copy might be needed. Because the delegate hides the assigned target, this issue is not visible.

  As a result, the _C++ delegates_ are move only. If you need multiple instances of the same delegate, just create multiple delegates and assign the same target.

- Why can't I directly assign a function to a _C++ delegate_, as for example with  `std::move_only_function`?
  Because it is less optimizable and may lead to less efficient code.

  - With direct assignment `delegate<void()> d = &function;`:  
    The address of the function or member function is passed to the assignment operator's function argument. Thus, it is only known during runtime.
  - When wrapped by a function object `delegate<void()> d = []() { function(); };`:  
    The function is now bound to the type of the function object. Therefore, it is known during compilation time.

- Why is the size for small object optimization `sizeof(void*)`?  
  An object pointer is already needed to store the dynamic allocated storage for function objects. If the size of the function object is less or equal `sizeof(void*)`, the space of the pointer can instead be used to store the function object in. This enables to small buffer optimize any lambda expression that captures a reference or a pointer. Through that reference/pointer any addionally needed data may be accessed.  
  As a result, dynamic allocation should be avoidable in any use case without increasing the size of the delegate.

- Why is the namespace called _rome_?  
  This has nothing to do with the Italian capiltal, it's just the initials of my name.

## Tests

The tests are free of warnings, tested with `-Werror -Wall -pedantic -Wextra`, on following compilers:

- Clang: 4.0.0 and 16.0.5
- GCC: 5.4.0 and 13.2.0
- MSVC: not yet tested

The tests can be found in [./test](./test).

Build and run the tests:

```bash
cmake -B build -D ROME_DELEGATES_BUILD_TESTS=ON -G Ninja
ninja -C build run_all_tests
```

`ROME_DELEGATES_BUILD_TESTS=ON` needs to be set to configure the test targets. They are hidden otherwise.

Test targets:

- `run_unittests` -- tests functionality and constraints of the delegates  
  Uses the library provided copy of the unittest framework [doctest][doctest] and the mocking framework [Trompeloeil].
- `run_compile_error_tests` -- tests whether compile errors are raised as expected  
  Requires Python 3 and pytest to be installed.
- `run_example_tests` -- runs the examples of the documentation and tests against the expected output  
  Requires Python 3 to be installed.
- `run_all_tests`: runs all the  tests above

## Feedback

Some request, a question or found a bug? Please [open an issue on GitLab](https://gitlab.com/rmettler/cpp_delegates/-/issues/new) or write me an email. I am also happy to hear from any positive or negative experience you had with this library.

## Motivation

The initial use case was to wire independent modules together. Let `A` and `B` be two modules. `A` is reactive and may produce the event `done`. If `A` produces the event `done`, `B` shall `start`.

```cpp
namespace module_a {
    struct A {
        /* something that produces the event `done` */
    };
}

namespace module_b {
    struct B {
        void start();
    };
}

void wire() {
    // connect them somehow
}
```

In OOP, this is commonly done by `A` having an association to an interface `I` and `B` realizing that interface. `B` is then injected into `A` by using `I`.  
It might look something like the following:

```cpp
namespace awkward {
    struct I {
        virtual void done() = 0;
    };
}

namespace module_a {
    struct A {
        awkward::I& done;
    };
}

namespace module_b {
    struct B : public awkward::I {
        void done() override;  // but should be called `start`
    };
}

void wire() {
    module_b::B b{};
    module_a::A a{b};
}
```

This has the following consequences:

- `I` needs to be stored somewhere, either in module `A`, module `B` or a third module.
  - If `I` is stored in `A` or `B`, one module depends on the other, they are not independent anymore.
  - If `I` is stored in a third module, that third module is created just for that purpose, while `A` and `B` still are not as independet anymore as without.
- By realizing `I` in `B`, `I` becomes a public interface of some class in `B`. However, the function names of `I` might be a bad fit for the public interface of `B`, so you need to add an extra class and hide it somehow, just to realize `I`.

A better solution is to use `std::function` for the event `done`:

```cpp
namespace module_a {
    struct A {
        std::function<void()> done;
    };
}

namespace b {
    struct B {
        void start();
    };
}

void wire() {
    module_a::A a{};
    module_b::B b{};
    a.done = [&b]() { b.start(); };
}
```

While already a lot better than the OOP interface headache, `std::function` isn't exactly lightweight. On memory restricted devices this might become a problem. Furthermore, `std::function` throws an exception if no target was assigned. This enforces to assign a target to any provided event, whether handling that event is of interest or not. And if the handling of an event is required, a compile error is preferable to a runtime exception.

So we need a lightweight solution, similar to `std::function`, that enables us to mark the handling of the event as optional or mandatory, with unhandeled optional events doing nothing and unhandeled mandatory events raising a compile error.

That's where my journey started and the reason for the [`rome::event_delegate`](doc/fwd_delegate.md) and the [`rome::command_delegate`](doc/fwd_delegate.md).

## Thanks

- Sergey Ryazanov for his incredible article [_The Impossibly Fast C++ Delegates_][impossDelegates]. He explains how you can use the fact that the address of any function can be passed as non-type template argument to create highly optimizable function delegates.
- Lee David for help finding more comprehensible names
- Matthias Deimbacher for the initial set up of the CI pipelines at gitlab

## References

1. Ryazanov, Sergey. “The Impossibly Fast C++ Delegates.” [_The Impossibly Fast C++ Delegates_][impossDelegates], CodeProject, 18 July 2005, www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates.
2. Cppreference. [_std::move_only_function_][std_move_only_function], cppreference.com, 14 November 2023, https://en.cppreference.com/w/cpp/utility/functional/move_only_function, CC-BY-SA.
3. Cppreference. [_std::function_][std_function], cppreference.com, 14 November 2023, https://en.cppreference.com/w/cpp/utility/functional/function, CC-BY-SA.
4. Kirilov, Viktor. [_doctest_][doctest], 15 March 2023, https://github.com/onqtam/doctest, MIT
5. Fahler, Björn. [_Trompeloeil_][trompeloeil], 21 July 2023, https://github.com/rollbear/trompeloeil, BSL-1.0

[impossDelegates]: https://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
[std_move_only_function]: https://en.cppreference.com/w/cpp/utility/functional/move_only_function
[std_function]: https://en.cppreference.com/w/cpp/utility/functional/function
[doctest]:  https://github.com/onqtam/doctest
[trompeloeil]: https://github.com/rollbear/trompeloeil
