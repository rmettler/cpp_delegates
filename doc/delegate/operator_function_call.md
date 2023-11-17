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
- [`rome::bad_delegate_call`](../bad_delegate_call.md) if `Behavior` != `rome::target_is_optional` and \*this is _empty_

_Note: A `rome::delegate` with `Behavior` == `rome::target_is_mandatory` can only become_ empty _after a move, i.e., after `auto y = std::move(x)`_ x _is_ empty.

## Examples

_See the code in [examples/empty_after_move.cpp](../examples/empty_after_move.cpp)._

```cpp
#include <iostream>
#include <rome/delegate.hpp>

int main() {
    rome::delegate<void(), rome::target_is_mandatory> d1 = []() { std::cout << "target called\n"; };
    d1();

    auto d2 = std::move(d1);
    try {
        d1();
    }
    catch (const rome::bad_delegate_call& e) {
        std::cout << e.what() << '\n';
    }
}
```

Output:

> target called  
> rome::bad_delegate_call
