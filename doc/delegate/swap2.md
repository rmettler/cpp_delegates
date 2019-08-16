# _rome::_ **swap**(rome::delegate)

```cpp
template<typename Ret, typename... Args>
constexpr void swap(delegate<Ret(Args...)>& lhs, delegate<Ret(Args...)>& rhs) noexcept;
```

Overloads the commonly used swap algorithm for `rome::delegate`. Exchanges the stored callable objects of both instances. Effectively calls `lhs.swap(rhs)`.

## Parameters

**lhs, rhs** -- The `rome::delegate` instances whose content to exchange.

## Return value

(none)

## Examples

> TODO
