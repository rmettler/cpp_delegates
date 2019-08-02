# _rome::delegate<Ret(Args...)>::_ **delegate**

```cpp
constexpr delegate() noexcept;                // (1)
constexpr delegate(std::nullptr_t) noexcept;  // (2)
constexpr delegate(const delegate& other);    // (3)
constexpr delegate(delegate&& other);         // (4)
```

Constructs a `rome::delegate`.

- **1, 2** -- Creates an empty delegate.
- **3** -- Copies the _target_ of other to the _target_ of `*this`. If other is empty, `*this` will be empty after the call too.
- **4** -- Moves the _target_ of other to the _target_ of `*this`. If other is empty, `*this` will be empty after the call too.

## Parameters

**other** -- Another `rome::delegate` object used to initialize `*this`.

## Exceptions

> TODO: check under what circumstances (3) and (4) may throw exceptions!  
> E.g. when the constructor throws that is used to copy or move a stored function object.

## Notes

TODO: just as template, there are none! -> remove

## Examples

> TODO: add **tested** examples for all of the constructors.  
> TODO: add link to the tests of those.
