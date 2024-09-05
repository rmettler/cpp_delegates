# _rome::_ **swap**(rome::delegate)

```cpp
template<typename Ret, typename... Args, typename Behavior>
void swap(
    delegate<Ret(Args...), Behavior>& lhs,
    delegate<Ret(Args...), Behavior>& rhs) noexcept;
```

Overloads the commonly used swap algorithm for `rome::delegate`. Exchanges the stored callable objects of both instances. Effectively calls [`lhs.swap(rhs)`](swap.md).

## Parameters

**lhs, rhs** -- The `rome::delegate` instances whose content to exchange.

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
