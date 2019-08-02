# _rome::delegate<Ret(Args...)>::_ **operator()**

```cpp
Ret operator()(Args... args) const;
```

Invokes the stored callable function _target_ with the parameters args.

## Parameters

**args** -- Parameters to pass to the stored callable function _target_.

## Return value

None if Ret is `void`. Otherwise the return value of the invocation of the stored callable function _target_.

## Exceptions

`rome::bad_delegate_call` if `*this` does not store a callable function target, i.e. `!*this == true`.

> TODO: add link from `rome::bad_delegate_call` to its markdown page

## Examples

> TODO
