# **operator==,!=**(rome::delegate)

```cpp
template<typename TgtReq, typename Ret, typename... Args>
constexpr bool operator==(
    const delegate<Ret(Args...),
    TgtReq>& d, std::nullptr_t) noexcept; // (1)

template<typename TgtReq, typename Ret, typename... Args>
constexpr bool operator==(
    std::nullptr_t,
    const delegate<Ret(Args...), TgtReq>& d) noexcept; // (2)

template<typename TgtReq, typename Ret, typename... Args>
constexpr bool operator!=(
    const delegate<Ret(Args...), TgtReq>& d,
    std::nullptr_t) noexcept; // (3)

template<typename TgtReq, typename Ret, typename... Args>
constexpr bool operator!=(
    std::nullptr_t,
    const delegate<Ret(Args...), TgtReq>& d) noexcept; // (4)
```

Compares a `rome::delegate` with null pointer.

## Parameters

**d** -- The `rome::delegate` to compare.

## Return value

- **1, 2** -- Returns `true` if the `rome::delegate` is _empty_. Same as `!d`.  
  Is alwas `true` if `TgtReq` == `rome::tgt_stat_req`.
- **3, 4** -- Returns `true` if the `rome::delegate` is not _empty_. Same ase `static_cast<bool>(d)`.  
  Is alwas `false` if `TgtReq` == `rome::tgt_stat_req`.

## Examples

> TODO
