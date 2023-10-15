# _rome::_ **fwd_delegate**

Defined in header [`<rome/delegate.hpp>`](../include/rome/delegate.hpp).

```cpp
template<typename Signature, typename Behavior = target_is_expected>
class fwd_delegate; // undefined

template<typename... Args, typename Behavior>
class fwd_delegate<void(Args...), Behavior>;

template<typename Signature>
using event_delegate = fwd_delegate<Signature, target_is_optional>;

template<typename Signature>
using command_delegate = fwd_delegate<Signature, target_is_mandatory>;
```

Instances of class template `rome::fwd_delegate` can store and invoke callable _targets_ such as functions, lambda expressions, std::function, other function objects, as well as static and non-static member functions.

The `rome::fwd_delegate` has identical functionality as [`rome::delegate`](delegate.md), but with the restriction that data can only be ___forwarded___. To ensure this, only function signatures with `void` return and arguments of immutable type are supported. E.g. the signature `void(const std::string&)` would work, while `void(int*)` or `bool()` would produce a compile error.

See [`rome::delegate`](delegate.md) for a description of the functionality.

## Template parameters

- `Args...`  
  The argument types of the _target_ beeing called (0 to N). Must be immutable.
- `Behavior`  
  Defines the behavior of an _empty_ `rome::delegate` being called. Defaults to `rome::target_is_expected`.
  
  The behavior can be chosen by declaring the delegate with one the following types:
  
  - `rome::target_is_expected`  
    A valid _target_ is expected to be assigned before the `rome::delegate` is called.  
    When an _empty_ `rome::delegate` is being called:
    - Throws a [`rome::bad_delegate_call`](delegate/bad_delegate_call.md) exception.
    - Instead calls [`std::terminate`](https://en.cppreference.com/w/cpp/error/terminate), if exceptions are disabled.
  - `rome::target_is_optional` _(only if `Ret`==`void`)_  
    Assigning a _target_ to the `rome::delegate` is optional. Calling an _empty_ delegate is well defined and directly returns without doing anything.  
    Compile error, if `Ret` != `void`.
  - `rome::target_is_mandatory`  
    Ensures by design that a `rome::delegate` cannot be _empty_. This has following consequences:
    - Default constructor is deleted. A new instance of `rome::delegate` can only be created by using one of the factory functions [create](delegate/create.md).
    - There is no possibility to drop a currently assigned _target_, though it can be overridden by assigning a new _target_.

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

```cpp
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <rome/delegate.hpp>

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

## See also

- [rome::delegate](delegate.md)  
  same as `rome::fwd_delegate` but without return and argument type restrictsions
- [std::function](https://en.cppreference.com/w/cpp/utility/functional/function)  
  wraps callable object of any type with specified function call signature
