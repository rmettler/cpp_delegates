# _rome::_ **delegate**

Defined in header [`<rome/delegate.hpp>`](../include/rome/delegate.hpp).

```cpp
template<typename, typename=throw_exception>
class delegate; // undefined

template<typename Ret, typename... Args, typename EmptyCallBehavior>
class delegate<Ret(Args...), EmptyCallBehavior>;
```

> TODO: change "EmptyCallBehavior" depending types to types like "required" and "optional" to show in the declaration whether the assignment of a _target_ is required by the interface or not. "optional" would then work for if the return type is `void` but would lead to undefined behavior otherwise! Thus default would be "required". Optional would still be allowd with non-void return types for areas where exceptions can't be used.

Class template `rome::delegate` wraps a call and hides the implementation of the _target_ being called. The main purpose for `rome::delegate` can be used to declare a call interface by only defining the required return and argument types.

Instances of `rome::delegate` can reference and invoke any callable _target_ -- functions, lambda expressions, other function objects, std::function, as well as static and non-static member functions.

The stored callable object is called the _target_ of `rome::delegate`. If a `rome::delegate` contains no _target_, it is called _empty_. Invoking the _target_ of an _empty_ `rome::delegate` by default results in a `rome::bad_delegate_call` exception being thrown. Other specifiable behaviors are ignoring the call or undefined behavior.

**Comparision to `std::function`**

- Similar declaration and usage as `std::function`.
- More efficient implementation than `std::function`.  
  > (TODO: reference a benchmark that shows this (when std::function uses SBO and when not)!)
- Doesn't take ownership of the _target_. Thus might reference a _target_ that doesn't live anymore, if not manually unassigned (may change in future).
- Already binds the object pointer for non-static member functions, so no `std::bind` is needed.
- Definable behavior when calling an _empty_ `rome::delegate`.

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
- `EmptyCallBehavior`  
  Defines the behavior when an _empty_ delegate is called. This parameter can be omitted.  
  
  Can be one of the following types, dependent on the return type `Ret`:  
  
  | `Ret`      | `EmptyCallBehavior`               | Behavior when calling an _empty_ delegate            |
  | ---------- | --------------------------------- | ---------------------------------------------------- |
  | `void`     | `rome::throw_exception` _(default)_ | throws a `rome::bad_delegate_call` exception               |
  |            | `rome::ignore_call`               | ignores the call and does nothing                    |
  |            | `rome::undefined_behavior`        | not available for void (compile error)               |
  | not `void` | `rome::throw_exception` _(default)_ | throws a `rome::bad_delegate_call` exception               |
  |            | `rome::ignore_call`               | not available non-void (compile error)               |
  |            | `rome::undefined_behavior`        | calls a nullptr and thus leads to undefined behavior |

## Member functions

- [constructor](delegate/constructor.md)  
  constructs a new `rome::delegate` instance
- [desctructor](delegate/destructor.md)  
  destroys a `rome::delegate` instance
- [operator=](delegate/operator_assignment.md)  
  assigns a new _target_
- [swap](delegate/swap.md)  
  swaps the contents
- [operator bool](delegate/operator_bool.md)  
  checks if a valid _target_ is contained
> TODO the *.md files from here!
- [operator()](delegate/operator_function_call.md)  
  invokes the _target_

## Non-member functions

- rome::make_delegate  
  factory function to create a new `rome::delegate` instance with assigned _target_
  > TODO
- operator==, operator!=  
  compares `rome::delegate` with nullptr
- std::swap  
  specialices the std::swap algorithm  
  > (TODO really do it this way round, add it to the std namespace???)

## Helper classes

> TODO are there any?

## TODO other sections?

> TODO

## Example

> TODO

## See also

> TODO

## TODO chapter for referencing cppreference.com

> TODO: because description is heavily influenced by their description of std::function
