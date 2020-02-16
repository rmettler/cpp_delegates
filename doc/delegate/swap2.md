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

> TODO
