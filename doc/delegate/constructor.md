# _rome::delegate<Ret(Args...)>::_ **delegate**

```cpp
constexpr delegate() noexcept;                 // (1)
constexpr delegate(std::nullptr_t) noexcept;   // (2)
constexpr delegate(delegate&& other) noexcept; // (3)
```

Constructs a `rome::delegate`.

- **1, 2** -- Creates an _empty_ delegate.
- **3** -- Moves the _target_ and the ownership of _target_ of other to `*this`. If other is _empty_, `*this` will be _empty_ after the call too. Leaves other in _empty_ state after the move.

Use [`rome::make_delegate`](../delegate/make_delegate.md) to assign a new _target_.

## Parameters

**other** -- Another `rome::delegate` object used to initialize `*this`.

## Notes

TODO: just as template, there are none! -> remove

## Examples

> TODO: add **tested** examples for all of the constructors.  
> TODO: add link to the tests of those.
