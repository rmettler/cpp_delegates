# _rome::delegate<Ret(Args...), Behavior>::_ **delegate**

If `Behavior` != `rome::target_is_mandatory`:

```cpp
constexpr delegate() noexcept;                   // (1)
constexpr delegate(std::nullptr_t) noexcept;     // (2)
delegate(delegate&& other) noexcept;             // (3)
delegate(const delegate& other) = delete;        // (4)
template<typename F>
delegate(F&& fnObject) noexcept(/*see below*/);  // (5)
```

If `Behavior` == `rome::target_is_mandatory`:

```cpp
delegate() = delete;                             // (1)
delegate(delegate&& other) noexcept;             // (3)
delegate(const delegate& other) = delete;        // (4)
template<typename F>
delegate(F&& fnObject) noexcept(/*see below*/);  // (5)
```

Constructs a `rome::delegate`.

- **1, 2** -- Creates an _empty_ delegate.  
  Only provided if `Behavior` != `rome::target_is_mandatory`.
- **3** -- Moves the _target_ of other to `*this`. If other is _empty_, `*this` will be _empty_ after the call too. Leaves other in _empty_ state after the move:
  - If `Behavior` != `rome::target_is_mandatory`:  
    After the move, other behaves as if default constructed.
  - If `Behavior` == `rome::target_is_mandatory`:  
    After the move, other behaves as if being default constructed with `Behavior` == `rome::target_is_expected` and throws a [`rome::bad_delegate_call`](delegate/bad_delegate_call.md) exception when called.
- **4** -- `rome::delegate` cannot be copied
- **5** -- Creates a delegate with its _target_ set to the passed function object.  
Let `T` be `std::decay_t<F>`, the type of the function object. The _target_ is constructed by `T(std::forward<F>(fnObject))`.
  - If `sizeof(T) <= sizeof(void*)` and `alignof(T) <= alignof(void*)`:  
    - The constructer is _noexcept_ if `T(std::forward<F>(fnObject))` is _noexcept_.
    - The _target_ is small object optimized, it is stored inside the `rome::delegate`. No dynamic allocation takes place.
  - Otherwise:
    - The constructor is not _noexcept_.
    - The _target_ is constructed in a dynamic allocated storage.

## Parameters

- `other`  
  Another `rome::delegate` object used to initialize `*this`.
- `F` - _template parameter_  
  The type by which the function object _target_ is passed.
- `fnObject`  
  The function object _target_ used to initialize `*this`.

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

- [create](delegate/create.md) - _static_  
  creates a new `rome::delegate` instance with given _target_ assigned
