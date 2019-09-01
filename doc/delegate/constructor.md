# _rome::delegate<Ret(Args...), TgtReq>::_ **delegate**

If `TgtReq` != `rome::tgt_stat_req`:

```cpp
constexpr delegate() noexcept;                 // (1)
constexpr delegate(std::nullptr_t) noexcept;   // (2)
template<typename T>
constexpr delegate(delegate<Ret(Args...), T>&& other) noexcept; // (3)
```

If `TgtReq` == `rome::tgt_stat_req`:

```cpp
delegate() = delete;
constexpr delegate(delegate&& other) noexcept; // (3)
```

Constructs a `rome::delegate`.

- **1, 2** -- Creates an _empty_ delegate.  
  Only available if `TgtReq` != `rome::tgt_stat_req`.
- **3** -- Moves the _target_ of other to `*this`. If other is _empty_, `*this` will be _empty_ after the call too. Leaves other in _empty_ state after the move.
  - If `TgtReq` of `*this` == `rome::tgt_stat_req`:  
    `TgtReq` of other must also be `rome::tgt_stat_req`
  - Otherwise:  
    `TgtReq` of other might be any valid type

Use [`rome::make_delegate`](../delegate/make_delegate.md) to assign a new _target_.

## Parameters

**other** -- Another `rome::delegate` object used to initialize `*this`.

## Examples

> TODO: add **tested** examples for all of the constructors.
