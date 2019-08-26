# **operator==,!=**(rome::delegate)

```cpp
template<typename Ret, typename... Args>
constexpr bool operator==(
    const delegate<Ret(Args...)>& d, std::nullptr_t) noexcept; // (1)

template<typename Ret, typename... Args>
constexpr bool operator==(
    std::nullptr_t, const delegate<Ret(Args...)>& d) noexcept; // (2)

template<typename Ret, typename... Args>
constexpr bool operator!=(
    const delegate<Ret(Args...)>& d, std::nullptr_t) noexcept; // (3)

template<typename Ret, typename... Args>
constexpr bool operator!=(
    std::nullptr_t, const delegate<Ret(Args...)>& d) noexcept; // (4)
```

Compares a `rome::delegate` with null pointer.

## Parameters

**d** -- The `rome::delegate` to compare.

## Return value

- **1, 2** -- Returns `true` if the `rome::delegate` is _empty_. Same as `!d`.
- **3, 4** -- Returns `true` if the `rome::delegate` is not _empty_. Same ase `static_cast<bool>(d)`.

## Examples

> TODO
