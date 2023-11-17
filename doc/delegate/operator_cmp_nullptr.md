# **operator==,!=**(rome::delegate)

```cpp
template<typename Ret, typename... Args, typename Behavior>
constexpr bool operator==(
    const delegate<Ret(Args...), Behavior>& d,
    std::nullptr_t) noexcept;                                    // (1)

template<typename Ret, typename... Args, typename Behavior>
constexpr bool operator==(
    std::nullptr_t,
    const delegate<Ret(Args...), Behavior>& d) noexcept;         // (2)

template<typename Ret, typename... Args, typename Behavior>
constexpr bool operator!=(
    const delegate<Ret(Args...), Behavior>& d,
    std::nullptr_t) noexcept;                                    // (3)

template<typename Ret, typename... Args, typename Behavior>
constexpr bool operator!=(
    std::nullptr_t,
    const delegate<Ret(Args...), Behavior>& d) noexcept;         // (4)
```

Compares a `rome::delegate` with null pointer.

## Parameters

**d** -- The `rome::delegate` to compare.

## Return value

- **1, 2** -- Returns `true` if the `rome::delegate` is _empty_. Same as `!d`.
- **3, 4** -- Returns `true` if the `rome::delegate` is not _empty_. Same ase `static_cast<bool>(d)`.

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
