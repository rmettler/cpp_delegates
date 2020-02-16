# **operator==,!=**(rome::delegate)

```cpp
template<typename Ret, typename... Args, typename Behavior>
constexpr bool operator==(
    const delegate<Ret(Args...), Behavior>& d,
    std::nullptr_t) noexcept;                                    // (1)

template<typename Ret, typename... Args, typename Behavior>
constexpr bool operator==(
    std::nullptr_t,
    const delegate<Ret(Args...), Behavior>& d) noexcept; // (2)

template<typename Ret, typename... Args, typename Behavior>
constexpr bool operator!=(
    const delegate<Ret(Args...), Behavior>& d,
    std::nullptr_t) noexcept;                                    // (3)

template<typename Ret, typename... Args, typename Behavior>
constexpr bool operator!=(
    std::nullptr_t,
    const delegate<Ret(Args...), Behavior>& d) noexcept; // (4)
```

Compares a `rome::delegate` with null pointer.

## Parameters

**d** -- The `rome::delegate` to compare.

## Return value

- **1, 2** -- Returns `true` if the `rome::delegate` is _empty_. Same as `!d`.  
  Is alwas `true` if `Behavior` == `rome::target_is_mandatory`.
- **3, 4** -- Returns `true` if the `rome::delegate` is not _empty_. Same ase `static_cast<bool>(d)`.  
  Is alwas `false` if `Behavior` == `rome::target_is_mandatory`.

## Examples

> TODO
