# _rome::delegate<Ret(Args...), Behavior>::_ **delegate**

If `Behavior` != `rome::target_is_mandatory`:

```cpp
constexpr delegate() noexcept;                // (1)
constexpr delegate(std::nullptr_t) noexcept;  // (2)
delegate(delegate&& other) noexcept;          // (3)
```

If `Behavior` == `rome::target_is_mandatory`:

```cpp
delegate() = delete;
delegate(delegate&& other) noexcept; // (3)
```

Constructs a `rome::delegate`.

- **1, 2** -- Creates an _empty_ delegate.  
  Only available if `Behavior` != `rome::target_is_mandatory`.
- **3** -- Moves the _target_ of other to `*this`. If other is _empty_, `*this` will be _empty_ after the call too. Leaves other in _empty_ state after the move:
  - If `Behavior` != `rome::target_is_mandatory`:  
    After the move, other behaves as if default constructed.
  - If `Behavior` == `rome::target_is_mandatory`:  
    After the move, other throws a [`rome::bad_delegate_call`](delegate/bad_delegate_call.md) exception when called.

Use [`create`](delegate/create.md) to assign a new _target_.

## Parameters

**other** -- Another `rome::delegate` object used to initialize `*this`.

## Examples

> TODO: add **tested** examples for all of the constructors.
