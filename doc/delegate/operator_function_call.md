# _rome::delegate<Ret(Args...), Behavior>::_ **operator()**

```cpp
Ret operator()(Args... args) const;
```

Invokes the stored callable function _target_ with the parameters args.

## Parameters

**args** -- Parameters to pass to the stored callable function _target_.

## Return value

None if Ret is `void`. Otherwise the return value of the invocation of the stored callable function _target_.

## Exceptions

- any exceptions thrown by the stored _target_
- [`rome::bad_delegate_call`](../bad_delegate_call.md) if `Behavior` == `rome::target_is_expected` and \*this is _empty_

## Examples

> TODO
