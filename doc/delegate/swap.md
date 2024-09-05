# _rome::delegate<Ret(Args...), Behavior>::_ **swap**

```cpp
void swap(delegate& other) noexcept;
```

Exchanges the stored callable objects of `*this` and other.

## Parameters

**other** -- Another `rome::delegate` to exchange the callable object with.

## Return value

(none)

## Examples

_See the code in [examples/swap.cpp](../examples/swap.cpp)._

```cpp
#include <iostream>
#include <utility>
#include <rome/delegate.hpp>

int main() {
    rome::delegate<void()> d1 = []() { std::cout << "1\n"; };
    rome::delegate<void()> d2 = []() { std::cout << "2\n"; };
    d1();
    d2();
    d1.swap(d2);
    d1();
    d2();
    {
        using std::swap;
        swap(d1, d2);
    }
    d1();
    d2();
}
```

Output:

> 1  
> 2  
> 2  
> 1  
> 1  
> 2
