# _rome::delegate<Ret(Args...)>::_ **operator=**

```cpp
constexpr delegate& operator=(const delegate& other);   // (1)
constexpr delegate& operator=(delegate&& other);        // (2)
constexpr delegate& operator=(std::nullptr_t) noexcept; // (3)
```

Assigns the _target_ of another `rome::delegate` or drops it.

- **1** -- Assigns a copy of _target_ of other to `*this`.
- **2** -- Moves the _target_ of other to `*this`.
- **3** -- Drops the current _target_. `*this` is _empty_ after the call.

See [`rome::make_delegate`](../delegate/make_delegate.md) for how to assign a new _target_.

## Parameters

**other** -- Another `rome::delegate` object to copy or move the target of.

## Return value

`*this`

## Exceptions

> TODO

## Examples

> TODO
