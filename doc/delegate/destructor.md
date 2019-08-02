# _rome::delegate<Ret(Args...)>::_ **~delegate**

```cpp
constexpr ~delegate();
```

Destroys the `rome::delegate` instance. If the `rome::delegate` is not _empty_, the _target_ is not destroyed.

> TODO: if ownership was transfered (e.g. by a move or an r-value passed function object), the _target_ IS destroyed also!

## Examples

> TODO: show examples where the target is destroyed and where not!
