# _rome::delegate<Ret(Args...), TgtReq>::_ **swap**

```cpp
template<typename T>
constexpr void swap(delegate<Ret(Args...), T>& other) noexcept;
```

Exchanges the stored callable objects of `*this` and other.

Compile error if `TgtReq` != `T` but either `TgtReq` or `T` are of type `rome::tgt_stat_req`.

## Parameters

**other** -- Another `rome::delegate` to exchange the callable object with.

## Return value

(none)

## Examples

> TODO
