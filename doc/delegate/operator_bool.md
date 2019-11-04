# _rome::delegate<Ret(Args...), ExpectedBehavior>::_ **operator bool**

```cpp
constexpr explicit operator bool() const noexcept;
```

Checks whether `*this` stores a callable function _target_, i.e. is not _empty_.

## Parameters

(none)

## Return value

`true` if `*this` stores a callable function target, `false` otherwise.

Is alwas `true` if `ExpectedBehavior` == `rome::target_is_mandatory`.

## Examples

> TODO
