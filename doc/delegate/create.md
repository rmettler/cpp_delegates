# _rome::delegate<Ret(Args...), Behavior>_ **create**

```cpp
template<Ret (*pFunction)(Args...)>
static constexpr delegate create() noexcept;                    // (1)

template<typename C, Ret (C::*pMethod)(Args...)>
static delegate create(C& object) noexcept;                     // (2)

template<typename C, Ret (C::*pMethod)(Args...) const>
static delegate create(const C& object) noexcept;               // (3)

template<typename F>
static delegate create(F&& fnObject) noexcept(/* see below*/);  // (4)
```

Factory function which creates a new `rome::delegate` from a callable _target_. The _target_ must be callable with the argument types `Args...` and return type `Ret`.

- **1** -- Initializes the _target_ with a free function or a static member function.
- **2** -- Initializes the _target_ with a non-const, non-static member function and the corresponding object's reference.
- **3** -- Initializes the _target_ with a const, non-static member function and the corresponding object's reference.
- **4** -- Initializes the _target_ with a function object that provides a compatible function call operator.  
Let `T` be `std::decay_t<F>`, the type of the function object. The _target_ is constructed by `T(std::forward<F>(fnObject))`.
  - If `sizeof(T) <= sizeof(void*)` and `alignof(T) <= alignof(void*)`:  
    - `create` is _noexcept_ if `T(std::forward<F>(fnObject))` is _noexcept_
    - The _target_ is small object optimized, it is stored inside the `rome::delegate`. No dynamic allocation takes place.
  - Otherwise:
    - `create` is not _noexcept_
    - The _target_ is constructed in a dynamic allocated storage.

## Parameters

- `pFunction` - _template parameter_  
  The function pointer or static member function pointer used to initialize the `rome::delegate` instance.
- `pMethod` - _template parameter_  
  The non-static member function pointer used to initialize the `rome::delegate` instance.
- `C` - _template parameter_  
  The class providing the non-static member function `pMethod`.
- `object`  
  The reference of the object of `C` used to initialize the `rome::delegate` instance together with the non-static member function pointer `pMethod`.
- `F` - _template parameter_  
  The type by which the function object is passed.
- `fnObject`  
  The function object used to initialize the `rome::delegate` instance.

## Return value

The newly created and initilaized `rome::delegate` instance.

## Exceptions

- **4** -- Let `T` be `std::decay_t<F>`, the type of the function object.  
  May throw any exception thrown by `T(std::forward<F>(fnObject))`.
  - If `sizeof(T) > sizeof(void*)` or `alignof(T) > alignof(void*)`:  
    Additionally, may throw any exception thrown by the dynamic allocation, including `std::bad_alloc`.

## Notes

By using the [constructor](delegate/constructor.md) over `create` to construct a `rome::delegate` has the benefit of better understandable compile errors, e.g., if a _target_ is of incompatible function call signature.  
See the example below for how to use either the constructor or the `create` function.

## Examples

_See the code in [examples/construct.cpp](../examples/construct.cpp)._

```cpp
#include <iostream>
#include <rome/delegate.hpp>

void function() {
    std::cout << "function\n";
}

struct C {
    void method() {
        std::cout << "method\n";
    }
    void const_method() const {
        std::cout << "const method\n";
    }
};

const auto fnObject = []() { std::cout << "function object\n"; };

int main() {
    {
        // const auto d = rome::delegate<void()>{&function};  // does not compile (1)
        const auto d1 = rome::delegate<void()>{[]() { function(); }};  // (2)
        const auto d2 = rome::delegate<void()>::create<&function>();
        d1();
        d2();
    }
    {
        C object;
        const auto d1 = rome::delegate<void()>{[&object]() { object.method(); }};  // (2)
        const auto d2 = rome::delegate<void()>::create<C, &C::method>(object);
        d1();
        d2();
    }
    {
        const C object;
        const auto d1 = rome::delegate<void()>{[&object]() { object.const_method(); }};  // (2)
        const auto d2 = rome::delegate<void()>::create<C, &C::const_method>(object);
        d1();
        d2();
    }
    {
        const auto d1 = rome::delegate<void()>{fnObject};
        const auto d2 = rome::delegate<void()>::create(fnObject);
        d1();
        d2();
    }
}
```

- **1** -- not supported because less optimizable, needs to be wrapped by a function object
- **2** -- non function objects wrapped by a function object

Output:

> function  
> function  
> method  
> method  
> const method  
> const method  
> function object  
> function object

## See also

- [constructor](delegate/constructor.md)  
  Constructs a new `rome::delegate` instance.
