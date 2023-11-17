# _rome::_ **fwd_delegate**

Defined in header [`<rome/delegate.hpp>`](../include/rome/delegate.hpp).

```cpp
template<typename Signature, typename Behavior = target_is_expected>
class fwd_delegate;  // undefined

template<typename... Args, typename Behavior>
class fwd_delegate<void(Args...), Behavior>;

template<typename Signature>
using event_delegate = fwd_delegate<Signature, target_is_optional>;

template<typename Signature>
using command_delegate = fwd_delegate<Signature, target_is_mandatory>;
```

Instances of class template `rome::fwd_delegate` can store and invoke any callable _target_ -- functions, lambda expressions, std::function, other function objects, as well as static and non-static member functions.

The `rome::fwd_delegate` has identical functionality as [`rome::delegate`](delegate.md), but with the restriction that data can only be **_forwarded_**. To ensure this, only function signatures with `void` return and arguments of immutable type are supported. E.g. the signature `void(const std::string&)` would work, while `void(int*)` or `bool()` would not compile.

See [`rome::delegate`](delegate.md) for a description of the functionality.

## Template parameters

- `Args...`  
  The argument types of the _target_ being called. Must be immutable.
- `Behavior`  
  Defines the behavior of an _empty_ `rome::fwd_delegate` being called. Defaults to `rome::target_is_expected`.
  
  The behavior can be chosen by declaring the delegate with one the following types:
  
  - `rome::target_is_expected`  
    A valid _target_ is expected to be assigned before the `rome::fwd_delegate` is called.  
    When an _empty_ `rome::fwd_delegate` is being called:
    - Throws a [`rome::bad_delegate_call`](./bad_delegate_call.md) exception.
    - Instead calls [`std::terminate`](https://en.cppreference.com/w/cpp/error/terminate), if exceptions are disabled.
  - `rome::target_is_optional`  
    Assigning a _target_ to the `rome::fwd_delegate` is optional. Calling an _empty_ delegate returns directly without doing anything.  
  - `rome::target_is_mandatory`  
    Prevents by design that a `rome::fwd_delegate` can be _empty_. This has following consequences:

    - Default constructor is deleted. A new instance of `rome::fwd_delegate` can only be created by passing a _target_ to the [constructor](delegate/constructor.md) or by using one of the factory functions [create](delegate/create.md).
    - There is no possibility to drop a currently assigned _target_, though it can be overridden by assigning a new _target_.

    _Note: The `rome::fwd_delegate` still becomes_ empty _after a move, i.e., after `auto y = std::move(x)`_ x _is_ empty _and behaves as if `Behavior` was set to `rome::target_is_expected`._

## Type aliases

- `rome::command_delegate`  
  This delegate was designed for event or message-driven architectures to command an action that shall happen. Because the execution of the command is mandatory, a target must be assigned during construction of the delegate and can only be overriden by another target afterwards.  
  A `rome::fwd_delegate` with `Behavior` set to `rome::target_is_mandatory`.
- `rome::event_delegate`  
  This delegate was designed for event or message-driven architectures, to notify about happened events. Thus, it is optional whether someone wants to listen to the event or not.  
  A `rome::fwd_delegate` with `Behavior` set to `rome::target_is_optional`.

## Member functions

See [`rome::delegate`](delegate.md)

## Non-member functions

See [`rome::delegate`](delegate.md)

## Example

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

## See also

- [rome::delegate](delegate.md)  
  The same as `rome::fwd_delegate` but without return and argument type restrictions.
- [std::move_only_function](https://en.cppreference.com/w/cpp/utility/functional/move_only_function) (C++23)  
  Wraps a callable object of any type with specified function call signature.
- [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) (C++11)  
  Wraps a callable object of any copy constructible type with specified function call signature.
