# _rome::delegate<Ret(Args...)>::_ **operator=**

```cpp
constexpr delegate& operator=(const delegate& other);   // (1)
constexpr delegate& operator=(delegate&& other);        // (2)
constexpr delegate& operator=(std::nullptr_t) noexcept; // (3)
```

Assigns a new _target_ to `rome::delegate`.

- **1** -- Assigns a copy of _target_ of other to `*this`.
- **2** -- Moves the _target_ of other to `*this`.
- **3** -- Drops the current _target_. `*this` is _empty_ after the call.

## Parameters

**other** -- Another `rome::delegate` object to copy or move the target of.

## Return value

`*this`

## Exceptions

> TODO

## Examples

> TODO
