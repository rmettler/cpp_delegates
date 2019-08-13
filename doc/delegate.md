# _rome::_ **delegate**

Defined in header [`<rome/delegate.hpp>`](../include/rome/delegate.hpp).

```cpp
template<typename, bool TargetRequired=true>
class delegate; // undefined

template<typename Ret, typename... Args, bool TargetRequired>
class delegate<Ret(Args...), TargetRequired>;
```

Class template `rome::delegate` wraps a call and hides the implementation of the _target_ being called.

Instances of `rome::delegate` can reference and invoke any callable _target_ -- functions, lambda expressions, other function objects, std::function, as well as static and non-static member functions.

The stored callable object is called the **_target_** of `rome::delegate`. If a `rome::delegate` contains no _target_, it is called **_empty_**. Invoking the _target_ of an _empty_ `rome::delegate` by default results in a `rome::bad_delegate_call` exception being thrown (see below).

Currently the `rome::delegate` is non-owning. E.g. if lambda expression is set as a _target_, the user has to make sure, that it is not destroyed as long as it is assigned to a `rome::delegate`.  
_In future `rome::delegate` will take the ownership of function objects and destroy them at the end of lifetime or if unassigned!_  
> TODO: add this to the specification of operator= and destructor

The size of a `rome::delegate` is 2 * `sizeof(void*)`.  
_As soon as `rome::delegate` is owning, the size will be incresed to  3 * `sizeof(void*)`._

> TODO: specify the possible heap allocation in make_delegate and the small buffer optimization for the capture of one pointer in lambda expression to prevent heap allocations here.

**Comparision to `std::function`**

- Similar declaration and usage as `std::function`.
- More efficient implementation than `std::function`.  
  > (TODO: reference a benchmark that shows this for both the libstdc++ and libc++ (when std::function uses SBO and when not)!)
- Already saves the object pointer for non-static member functions, so no `std::bind` or additional lambda is needed.
- `rome::delegate` can be configured to ignore a call to an _empty_ `rome::delegate`, where `std::function` always throws an exception.
- Size of `rome::delegate` is specified.

> **Note**
>
> At the moment `rome::delegate` does not own the assigned _target_ and does not delete it during destruction. However, there are plans to enhance the delegate to be able to own a _target_ and automatically destroy it.
>
> It will also use small buffer optimization to ensure that at least a lambda with captured pointer can be stored in the delegate without a heap allocation needed, while still ensuring a lowest possible footprint. This size will be specified in the documentation (probably relative to the size of `void`) and chosen as small as possible.
>
> A constexpr function will be provided to check during compile time whether a certain lambda or object can be contained within the provided buffer of `rome::delegate` or whether it needs a heap-allocation during assignment.

## Template parameters

- `Ret`  
  The return type of the _target_ beeing called.
- `Args...`  
  The argument types of the _target_ beeing called (0 to many).
- `TargetRequired`  
  Whether it is required to assign a valid _target_ to the `rome::delegate` before it is called. Defaults to `true`.
  
  This parameter only has an effect if an _empty_ `rome::delegate` is called:
  
  - `TargetRequired` == `true`:
    - Throws a `rome::bad_delegate_call` exception.
    - Instead calls [`std::terminate`](https://en.cppreference.com/w/cpp/error/terminate) if exceptions are disabled.
  - `TargetRequired` == `false` and `Ret` == `void`:  
    Ignores the call and returns directly.
  - `TargetRequired` == `false` and `Ret` != `void`:  
    Compile error, because it can't return anything.

## Member functions

- [constructor](delegate/constructor.md)  
  constructs a new `rome::delegate` instance
- [desctructor](delegate/destructor.md)  
  destroys a `rome::delegate` instance
- [operator=](delegate/operator_assignment.md)  
  assigns the _target_ of another `rome::delegate` or drops current _target_
- [swap](delegate/swap.md)  
  swaps the contents
- [operator bool](delegate/operator_bool.md)  
  checks if a valid _target_ is contained
- [operator==, operator!=](delegate/operator_cmp_delegate.md)  
  shallow comparision with another `rome::delegate` instance
  > TODO
- [operator()](delegate/operator_function_call.md)  
  invokes the _target_
  > TODO

## Non-member functions

- [rome::make_delegate](delegate/make_delegate.md)  
  creates a new `rome::delegate` instance with an assigned new _target_
  > TODO
- [operator==, operator!=](delegate/operator_cmp_nullptr.md)  
  compares `rome::delegate` with nullptr
  > TODO
- [swap](delegate/swap.md)  
  swaps the contents of two `rome::delegate` instances  
  > TODO

## Helper classes

> TODO are there any?

## TODO other sections?

> TODO

## Notes

`rome::delegate` was originally implemented as a solution for a more transparent and flexible declaration of call interfaces, e.g. for callback functions. For this purpose, however, the more restricted [`rome::event_delegate`](event_delegate.md) might be a better solution.

If the assignment of a _target_ is required, calling the pure virtual function of an injected [interface class](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Interface_Class) might be a better solution. In this case the compiler will check whether the pure virtual function is implemented and thus enforces the assignment of a _target_.

## Example

> TODO

## See also

> TODO (std::function)

## TODO chapter for referencing cppreference.com

> TODO: because description is heavily influenced by their description of std::function
