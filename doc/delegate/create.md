# _rome::delegate<Ret(Args...)>_ **create**

```cpp
template<Ret (*pTarget)(Args...)>
static constexpr delegate create() noexcept;             // (1)

template<typename C, Ret (C::*pTarget)(Args...)>
static constexpr delegate create(C& obj) noexcept;       // (2)

template<typename C, Ret (C::*pTarget)(Args...) const>
static constexpr delegate create(const C& obj) noexcept; // (3)

template<typename T>
static delegate create(T target) noexcept(
    std::is_nothrow_move_constructible<T>::value);       // (4)

template<typename T>
static delegate create(T target);                        // (5)
```

> TODO: was ist z.B. mit member value, der operator() unterstützt? -> testen

> TODO: check whether return types must be delegate<Ret(Args...), rome::tgt_stat_req> or another type

Factory method which creates a `rome::delegate` instance from a variety of _target_ sources. The passed _target_ must be callable with the argument types `Args...` and return type `Ret`.

- **1** -- Initializes the _target_ with a global function or a static member function.
- **2** -- Initializes the _target_ with a non-const, non-static member function and its object's reference.
- **3** -- Initializes the _target_ with a const, non-static member function and its object's reference.
- **4** -- Initializes the _target_ with `std::move(target)` and stores it in the local buffer of the `rome::delegate` instance.  
Is used if `sizeof(T) <= sizeof(void*)` and `alignof(T) <= sizeof(void*)`.
- **5** -- Initializes the _target_ with `std::move(target)` and allocates it on the heap.  
Is used instead of **4** if `sizeof(T) > sizeof(void*)` or `alignof(T) > sizeof(void*)`.

See also [`rome::make_delegate`](../delegate/make_delegate.md).
> TODO: explain difference

## Parameters

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
