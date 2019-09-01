# _rome::_ **swap**(rome::delegate)

```cpp
template<typename TgtReqLhs, typename TgtReqRhs, typename Ret, typename... Args>
constexpr void swap(
    delegate<Ret(Args...), TgtReqLhs>& lhs,
    delegate<Ret(Args...), TgtReqRhs>& rhs) noexcept;
```

Overloads the commonly used swap algorithm for `rome::delegate`. Exchanges the stored callable objects of both instances. Effectively calls [`lhs.swap(rhs)`](swap.md).

## Parameters

**lhs, rhs** -- The `rome::delegate` instances whose content to exchange.

## Return value

(none)

## Examples

> TODO
