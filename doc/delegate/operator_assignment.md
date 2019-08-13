# _rome::delegate<Ret(Args...)>::_ **operator=**

```cpp
constexpr delegate& operator=(delegate&& other) noexcept; // (1)
constexpr delegate& operator=(std::nullptr_t) noexcept;   // (2)
```

Assigns the _target_ of another `rome::delegate` or drops it.

- **1** -- Moves the _target_ and the ownership of _target_ of other to `*this`. If other is _empty_, `*this` will be _empty_ after the call too. Leaves other in _empty_ state after the move.
- **2** -- Drops the current _target_. `*this` is _empty_ after the call.

Use [`rome::make_delegate`](../delegate/make_delegate.md) to assign a new _target_.

## Parameters

**other** -- Another `rome::delegate` object to copy or move the target of.

## Return value

`*this`

## Examples

> TODO
