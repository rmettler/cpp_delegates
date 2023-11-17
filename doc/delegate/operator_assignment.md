# _rome::delegate<Ret(Args...), Behavior>::_ **operator=**

If `Behavior` != `rome::target_is_mandatory`:

```cpp
delegate& operator=(delegate&& other) noexcept;       // (1)
delegate& operator=(const delegate& other) = delete;  // (2)
delegate& operator=(std::nullptr_t) noexcept;         // (3)
template<typename F>
delegate(F&& fnObject) noexcept(/*see below*/);       // (4)
```

If `Behavior` == `rome::target_is_mandatory`:

```cpp
delegate& operator=(delegate&& other) noexcept;       // (1)
delegate& operator=(const delegate& other) = delete;  // (2)
template<typename F>
delegate(F&& fnObject) noexcept(/*see below*/);       // (4)
```

Assigns or drops a _target_.

- **1** -- Moves the _target_ of other to `*this`. If `*this` is not _empty_, destroys the currently stored _target_ first. If other is _empty_, `*this` will be _empty_ after the call too. Leaves other in _empty_ state after the move:
  - If `Behavior` != `rome::target_is_mandatory`:  
    After the move, other behaves as if default constructed.
  - If `Behavior` == `rome::target_is_mandatory`:  
    After the move, other behaves as if being default constructed with `Behavior` == `rome::target_is_expected` and throws a [`rome::bad_delegate_call`](delegate/bad_delegate_call.md) exception when called.
- **2** -- `rome::delegate` cannot be copied
- **3** -- Drops the stored _target_ and destroys it. `*this` is _empty_ after the call.  
  Only available if `Behavior` != `rome::target_is_mandatory`.
- **4** -- Assigns the passed function object as a new _target_ to `*this`. If `*this` is not _empty_, destroys the currently stored _target_ first.  
Let `T` be `std::decay_t<F>`, the type of the function object. The newly assigned _target_ is constructed by `T(std::forward<F>(fnObject))`.
  - If `sizeof(T) <= sizeof(void*)` and `alignof(T) <= alignof(void*)`:  
    - The assignment is _noexcept_ if `T(std::forward<F>(fnObject))` is _noexcept_.
    - The _target_ is small object optimized, it is stored inside the `rome::delegate`. No dynamic allocation takes place.
  - Otherwise:
    - The assignment is not _noexcept_.
    - The _target_ is constructed in a dynamic allocated storage.

## Parameters

- `other`  
  Another `rome::delegate` object to move the _target_ of.
- `F` - _template parameter_  
  The type by which the function object _target_ is passed.
- `fnObject`  
  The function object _target_ to assign to `*this`.

## Return value

`*this`

## Examples

_See the code in [examples/assign.cpp](../examples/assign.cpp)._

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
};

const auto fnObject = []() { std::cout << "function object\n"; };

int main() {
    rome::delegate<void(), rome::target_is_optional> d;
    C object;
    d();
    d = []() { function(); };
    d();
    d = nullptr;
    d();
    d = [&object]() { object.method(); };
    d();
    d = rome::delegate<void(), rome::target_is_optional>{};
    d();
    d = fnObject;
    d();
}
```

Output:

> function  
> method  
> function object
