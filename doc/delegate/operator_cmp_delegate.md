# _rome::delegate<Ret(Args...)>::_ **operator ==,!=**

```cpp
constexpr bool operator==(const delegate& other) const noexcept;
constexpr bool operator!=(const delegate& other) const noexcept;
```

Compares whether `*this` stores the same callable function _target_ as other.

## Parameters

**other** -- Another `rome::delegate` instance to compare with.

## Return value

**1** -- `true` if `*this` stores the same callable function _target_ as other
**2** -- `true` if `*this` stores the another callable function _target_ as other

## Examples

> TODO
