# _rome::_ **delegate**

Defined in header [`<rome/delegate.hpp>`](../include/rome/delegate.hpp).

```cpp
template<typename Signature, bool TargetRequired=true>
class delegate; // undefined

template<bool TargetRequired, typename Ret, typename... Args>
class delegate<Ret(Args...), TargetRequired>;
```

Instances of class template `rome::delegate` can store and invoke any callable _target_ -- functions, lambda expressions, other function objects, std::function, as well as static and non-static member functions.

The stored callable object is called the **_target_** of `rome::delegate`. If a `rome::delegate` contains no _target_, it is called **_empty_**. Invoking the _target_ of an _empty_ `rome::delegate` by default results in a [`rome::bad_delegate_call`](delegate/bad_delegate_call.md) exception being thrown (see below).

To assign a new _target_, a new `rome::delegate` needs to be constructed using [`rome::make_delegate`](delegate/make_delegate.md). If the size of the _target_ exceeds `sizeof(void*)`, a heap allocation might be needed during this process.

The size of a `rome::delegate` is 3 \* `sizeof(void*)`.

## Template parameters

- `Ret`  
  The return type of the _target_ beeing called.
- `Args...`  
  The argument types of the _target_ beeing called (0 to many).
- `TargetRequired`  
  Whether it is required to assign a valid _target_ to the `rome::delegate` before it is called. Defaults to `true`.
  
  This parameter only has an effect when an _empty_ `rome::delegate` is being called:
  
  - `TargetRequired` == `true`:
    - Throws a [`rome::bad_delegate_call`](delegate/bad_delegate_call.md) exception.
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
  assigns the _target_ of another `rome::delegate` or just drops the stored _target_
- [swap](delegate/swap.md)  
  swaps the contents
- [operator bool](delegate/operator_bool.md)  
  checks if a valid _target_ is contained
- [operator==, operator!=](delegate/operator_cmp_delegate.md)  
  shallow comparision with another `rome::delegate` instance
- [operator()](delegate/operator_function_call.md)  
  invokes the _target_
- [create](delegate/create.md) - _static_  
  creates a new `rome::delegate` instance with given _target_ assigned

## Non-member functions

- [rome::make_delegate](delegate/make_delegate.md)  
  creates a new `rome::delegate` instance with an assigned new _target_
- [rome::swap](delegate/swap2.md)  
  swaps the contents of two `rome::delegate` instances
- [operator==, operator!=](delegate/operator_cmp_nullptr.md)  
  compares `rome::delegate` with nullptr

## Notes

`rome::delegate` was originally implemented as a solution for a more transparent and flexible declaration of call interfaces, e.g. for callback functions. For this purpose, however, the more restricted [`rome::event_delegate`](event_delegate.md) might be a better solution.

If the assignment of a _target_ is required, calling the pure virtual function of an injected [interface class](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Interface_Class) might be a better solution. In this case the compiler will check whether the pure virtual function is implemented and thus enforces the assignment of a _target_.

> **Comparision with `std::function`**
>
> - Similar declaration and usage as `std::function`.
> - More efficient implementation than `std::function`.  
>   > (TODO: reference a benchmark that shows that this is true for both  the libstdc++ and libc++ (when std::function uses SBO and when not)!)
> - Already stores the object pointer for non-static member functions, so  no `std::bind` or additional lambda is needed.
> - `rome::delegate` can be configured to ignore a call to an _empty_ > `rome::delegate`, where `std::function` always throws an exception.
> - Size of `rome::delegate` is specified.

## Example

> TODO

## See also

- [rome::function_ptr_t](function_ptr.md)  
  helper type to define a function pointer
- [rome::member_function_ptr_t](function_ptr.md)  
  helper type to define a pointer of a non-static member function
- [std::function](https://en.cppreference.com/w/cpp/utility/functional/function)  
  wraps callable object of any type with specified function call signature

## TODO chapter for referencing cppreference.com

> TODO: because description is heavily influenced by their description of std::function
