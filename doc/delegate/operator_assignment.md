# _rome::delegate<Ret(Args...), ExpectedBehavior>::_ **operator=**

If `ExpectedBehavior` != `rome::target_is_mandatory`:

```cpp
delegate& operator=(delegate&& other) noexcept;          // (1)
constexpr delegate& operator=(std::nullptr_t) noexcept;  // (2)
```

If `ExpectedBehavior` == `rome::target_is_mandatory`:

```cpp
delegate& operator=(delegate&& other) noexcept;          // (1)
```

Assigns the _target_ of another `rome::delegate` or drops it.

- **1** -- Moves the _target_ of other to `*this`. If `*this` is not _empty_, destroys the currently stored _target_ first. If other is _empty_, `*this` will be _empty_ after the call too. Leaves other in _empty_ state after the move:
  - If `ExpectedBehavior` != `rome::target_is_mandatory`:  
    After the move, other behaves as if default constructed.
  - If `ExpectedBehavior` == `rome::target_is_mandatory`:  
    After the move, other behaves as if a `rome::delegate<Ret(Args...), rome::target_is_expected>` was default constructed and throws a [`rome::bad_delegate_call`](delegate/bad_delegate_call.md) exception when called.
- **2** -- Drops the stored _target_ and destroys it. `*this` is _empty_ after the call.  
  Only available if `ExpectedBehavior` != `rome::target_is_mandatory`.

Use [`create`](delegate/create.md) to assign a new _target_.

## Parameters

**other** -- Another `rome::delegate` object to copy or move the target of.

## Return value

`*this`

## Examples

> TODO
