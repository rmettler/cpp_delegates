# _rome::delegate<Ret(Args...), Behavior>::_ **operator bool**

```cpp
constexpr explicit operator bool() const noexcept;
```

Checks whether `*this` stores a callable function _target_, i.e. is not _empty_.

## Return value

`true` if `*this` stores a callable function target, `false` otherwise.

## Examples

_See the code in [examples/check_empty.cpp](../examples/check_empty.cpp)._

```cpp
#include <iostream>
#include <rome/delegate.hpp>

int main() {
    const rome::delegate<void()> empty{};
    const rome::delegate<void()> assigned{[]() {}};
    std::cout << "empty:\n";
    std::cout << std::boolalpha << static_cast<bool>(empty) << ", " << (empty == nullptr) << ", "
              << (nullptr == empty) << '\n';
    std::cout << "assigned:\n";
    std::cout << std::boolalpha << static_cast<bool>(assigned) << ", " << (assigned == nullptr)
              << ", " << (nullptr == assigned) << '\n';
}
```

Output:

> empty:  
> false, true, true  
> assigned:  
> true, false, false
