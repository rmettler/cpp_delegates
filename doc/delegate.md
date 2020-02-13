# _rome::_ **delegate**

Defined in header [`<rome/delegate.hpp>`](../include/rome/delegate.hpp).

```cpp
template<typename Signature, typename Behavior = target_is_expected>
class delegate; // undefined

template<typename Ret, typename... Args, typename Behavior>
class delegate<Ret(Args...), Behavior>;
```

Instances of class template `rome::delegate` can store and invoke any callable _target_ -- functions, lambda expressions, std::function, other function objects, as well as static and non-static member functions.

The stored callable object is called the **_target_** of `rome::delegate`. If a `rome::delegate` contains no _target_, it is called **_empty_**.

Invoking the _target_ of an _empty_ `rome::delegate` results in a behavior configurable by the `Behavior` template parameter (see below). By default a [`rome::bad_delegate_call`](delegate/bad_delegate_call.md) exception is thrown.

To assign a new _target_, a new `rome::delegate` needs to be constructed using [`create`](delegate/create.md). If a function object is assigned and its size exceeds `sizeof(void*)`, a heap allocation is needed during assignment. Smaller function objects and all other _targets_ are guaranteed to be embedded in the local memory of the `rome::delegate`. Thus, it is possible to efficiently assign a _target_ from a lambda expression with one captured pointer.

The size of a `rome::delegate` is the size of an object pointer plus twice the size of a function pointer:  
`sizeof(rome::delegate<Ret(Args...), Behavior>) == sizeof(void*) + 2*sizeof(void (*)())`

## Template parameters

- `Ret`  
  The return type of the _target_ beeing called.
- `Args...`  
  The argument types of the _target_ beeing called (0 to N).
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

## Member functions

- [constructor](delegate/constructor.md)  
  constructs a new `rome::delegate` instance
- [desctructor](delegate/destructor.md)  
  destroys a `rome::delegate` instance
- [operator=](delegate/operator_assignment.md)  
  assigns the _target_ of another `rome::delegate` or just drops the stored _target_
- [swap](delegate/swap.md)  
  swaps the contents
- [operator bool](delegate/operator_bool.md)  
  checks if a valid _target_ is contained
- [operator()](delegate/operator_function_call.md)  
  invokes the _target_
- [create](delegate/create.md) - _static_  
  creates a new `rome::delegate` instance with given _target_ assigned

## Non-member functions

- [rome::swap](delegate/swap2.md)  
  swaps the contents of two `rome::delegate` instances
- [operator==, operator!=](delegate/operator_cmp_nullptr.md)  
  compares `rome::delegate` with nullptr

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
    rome::delegate<void(const std::vector<int>&), rome::target_is_mandatory> onAddCommandRead;
    rome::delegate<void(const std::vector<int>&), rome::target_is_expected>  onSubtractCommandRead;
    rome::delegate<void(const std::string&),      rome::target_is_optional>  onParseError;
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
    // Calls the target assigned to the first delegate. It was mandatory to assign a target to this delegate and would have led to a compiler error otherwise.
    try {
        std::cout << "cmd2:" << '\n';
        cp.processCommand(cmd2);
        // Calls the second delegate with no assigned target. For this delegate, however, it is expected that a target is assigned before call. -> an exception is thrown
    }
    catch (const rome::bad_delegate_call& ex) {
        std::cout << ex.what() << '\n';
    }
    std::cout << "cmd3:" << '\n';
    cp.processCommand(cmd3);
    // Calls the third delegate with no assigned target. For this it is optional that a target is assigned before call. -> nothing happens
}
```

Console output:

> cmd1:  
> sum = 6  
> cmd2:  
> rome::bad_delegate_call  
> cmd3:

## See also

- [rome::fwd_delegate](fwd_delegate.md)  
  same as `rome::delegate` but restricts data to be forwarded only
- [std::function](https://en.cppreference.com/w/cpp/utility/functional/function)  
  wraps callable object of any type with specified function call signature

## Remarks

This documentation is heavily influenced by the documentation of [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) at [cppreference.com](https://en.cppreference.com).
