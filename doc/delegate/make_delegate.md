# _rome::_ **make_delegate**

Defined in header [`<rome/delegate.hpp>`](../include/rome/delegate.hpp).

```cpp
template<typename Signature, Signature pTarget>
constexpr auto make_delegate() noexcept;                 // (1)

template<typename Signature, Signature pTarget, typename C>
constexpr auto make_delegate(C& obj) noexcept;           // (2)

template<typename Signature, Signature pTarget, typename C>
constexpr auto make_delegate(const C& obj) noexcept;     // (3)

template<typename T>
auto make_delegate(T target) noexcept(
    std::is_nothrow_move_constructible<T>::value);       // (4)

template<typename T>
auto make_delegate(T target);                            // (5)
```

> TODO: was ist z.B. mit member value, der operator() unterstützt? -> testen

Factory method which creates a `rome::delegate` instance from a variety of _target_ sources. Can be used in a way that the argument and return types of the _target_ are identified by the compiler.

- **1** -- Initializes the _target_ with a global function or a static member function.
- **2** -- Initializes the _target_ with a non-const, non-static member function and its object's reference.
- **3** -- Initializes the _target_ with a const, non-static member function and its object's reference.
- **4** -- Initializes the _target_ with `std::move(target)` and stores it in the local buffer of the `rome::delegate` instance. Is used if `sizeof(T) <= sizeof(void*)` and `alignof(T) <= sizeof(void*)`.
- **5** -- Initializes the _target_ with `std::move(target)` and allocates it on the heap. Is used instead of **4** if `sizeof(T) > sizeof(void*)` or `alignof(T) > sizeof(void*)`.

Internally uses [`rome::delegate<Ret(Args...)>::create`](../delegate/create.md) to create the instance.
> TODO explain difference

## Parameters

- `Signature` - _template parameter_  
  The function signature of the _target_. Needs to be of following form:
  - **1** -- `Ret (*)(Args...)`
  - **2** -- `Ret (C::*)(Args...)`
  - **3** -- `Ret (C::*)(Args...) const`
- `pTarget` - _template parameter_  
  The function pointer or member function pointer used to initialize the `rome::delegate` instance.
- `C` - _template parameter_  
  The class type of the non-static member function.
- `obj`  
  A reference of the object used to initialize the `rome::delegate` instance together with a member function pointer.
- `T` - _template parameter_  
  Type of the callable used to initialize the `rome::delegate` instance.
- `target`  
  A callable used to initialize the `rome::delegate` instance.

## Return value

The newly created and initilaized `rome::delegate` instance.

## Exceptions

- **4** -- May throw any exception thrown by the move constructor of `T`.
- **5** -- May throw `std::bad_alloc` or any exception thrown by the move constructor of `T`.

## Examples

> TODO
