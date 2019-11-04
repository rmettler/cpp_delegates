# _rome::_ **delegate**

Defined in header [`<rome/delegate.hpp>`](../include/rome/delegate.hpp).

```cpp
template<typename Signature, typename ExpectedBehavior = target_is_expected>
class delegate; // undefined

template<typename Ret, typename... Args, typename ExpectedBehavior>
class delegate<Ret(Args...), ExpectedBehavior>;
```

Instances of class template `rome::delegate` can store and invoke any callable _target_ -- functions, lambda expressions, std::function, other function objects, as well as static and non-static member functions.

The stored callable object is called the **_target_** of `rome::delegate`. If a `rome::delegate` contains no _target_, it is called **_empty_**.

Invoking the _target_ of an _empty_ `rome::delegate` results in a behavior configurable by the `ExpectedBehavior` template parameter (see below). By default a [`rome::bad_delegate_call`](delegate/bad_delegate_call.md) exception is thrown.

To assign a new _target_, a new `rome::delegate` needs to be constructed using [`create`](delegate/create.md). If a function object is assigned and its size exceeds `sizeof(void*)`, a heap allocation might be needed during assignment. Smaller function objects and all other _targets_ are guaranteed to be embedded in the local memory of the `rome::delegate`. Thus it is possible to efficiently assign a _target_ from a lambda expression with one captured pointer.

The size of a `rome::delegate` is the size of an object pointer plus twice the size of a function pointer:  
`sizeof(rome::delegate<Ret(Args...), ExpectedBehavior>) == sizeof(void*) + 2*sizeof(void (*)())`

## Template parameters

- `Ret`  
  The return type of the _target_ beeing called.
- `Args...`  
  The argument types of the _target_ beeing called (0 to N).
- `ExpectedBehavior`  
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
    - Default constructor is deleted. A new instance of `rome::delegate` can only be created by using one of the factory functions [create](delegate/create.md) or [rome::make_delegate](delegate/make_delegate.md).
    - Move construction and move assignment only accept delegates which are also declared with `rome::target_is_mandatory`.
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

- [rome::make_delegate](delegate/make_delegate.md)  
  creates a new `rome::delegate` instance with given _target_ assigned
- [rome::swap](delegate/swap2.md)  
  swaps the contents of two `rome::delegate` instances
- [operator==, operator!=](delegate/operator_cmp_nullptr.md)  
  compares `rome::delegate` with nullptr

> **Comparision with `std::function`, TODO move this!**
>
> - Similar declaration and usage as `std::function`.
> - Trades a bit of usability for a more efficient implementation than `std::function`.  
>   > (TODO: reference a benchmark that shows that this is true for both  the libstdc++ and libc++ (when std::function uses SBO and when not)!)
> - Already stores the object pointer for non-static member functions, so  no `std::bind` or additional lambda is needed.
> - `rome::delegate` can be configured to ignore a call to an _empty_ > `rome::delegate`, where `std::function` always throws an exception.
> - Size of `rome::delegate` is specified.  
> - Definable behavior when calling an empty `rome::delegate`, `std::function` always throws an exception.
>   > (TODO: do this in tabular form listing the features)

## Example

```cpp
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "rome/delegate.hpp"

struct CommandProcessor {
    rome::delegate<void(const std::vector<int>&), rome::target_is_mandatory> onAddCommandRead;
    rome::delegate<void(const std::vector<int>&), rome::target_is_expected> onSubtractCommandRead;
    rome::delegate<void(const std::vector<int>&), rome::target_is_optional> onProductCommandRead;
    void processCommand(const std::string& line) {
        std::istringstream iss{line.substr(2)};
        const std::vector<int> args{std::istream_iterator<int>{iss}, std::istream_iterator<int>{}};
        if ('+' == line.at(0)) {
            onAddCommandRead(args);
        }
        else if ('-' == line.at(0)) {
            onSubtractCommandRead(args);
        }
        else if ('*' == line.at(0)) {
            onProductCommandRead(args);
        }
    }
    CommandProcessor(rome::delegate<void(const std::vector<int>&), rome::target_is_mandatory>&& dgt)
        : onAddCommandRead{std::move(dgt)} {
    }
};

int main() {
    CommandProcessor cp{decltype(cp.onAddCommandRead)::create([](const std::vector<int>& args) {
        const auto result = std::accumulate(args.begin(), args.end(), 0);
        std::cout << "sum = " << result << '\n';
    })};
    std::string cmd1{"+ 1 2 3"};
    std::string cmd2{"- 1 2 3"};
    std::string cmd3{"* 1 2 3"};
    std::cout << "cmd1:" << '\n';
    cp.processCommand(cmd1);  // calls the delegate mandatory to be passed during construction
    try {
        std::cout << "cmd2:" << '\n';
        cp.processCommand(cmd2);  // calls expected but unassigned delegate
    }
    catch (const rome::bad_delegate_call& ex) {
        std::cout << ex.what() << '\n';
    }
    std::cout << "cmd3:" << '\n';
    cp.processCommand(cmd3);  // calls optional and unassigned delegate
}
```

Console output:

> cmd1:  
> sum = 6  
> cmd2:  
> rome::bad_delegate_call  
> cmd3:

## See also

- [rome::function_ptr_t](function_ptr.md)  
  helper type to define a function pointer
- [rome::member_function_ptr_t](function_ptr.md)  
  helper type to define a pointer to a non-static member function
- [std::function](https://en.cppreference.com/w/cpp/utility/functional/function)  
  wraps callable object of any type with specified function call signature

## TODO chapter for referencing cppreference.com

> TODO: because description is heavily influenced by their description of std::function
