# _rome::delegate<Ret(Args...), Behavior>::_ **~delegate**

```cpp
~delegate();
```

Destroys the `rome::delegate` instance. If the `rome::delegate` instance took the ownership of a _target_, the _target_ is destroyed too.

## Examples

```cpp
#include <iostream>
#include <rome/delegate.hpp>

struct F {
    ~F() {
        std::cout << "~F()\n";
    }
    void operator()() const {
    }
};

int main() {
    rome::delegate<void()> d = F{};
    std::cout << "end of main\n";
}
```

Output:

> ~F()  
> end of main  
> ~F()

The first `~F()` comes from the temporary object passed to the `rome::delegate`, the second from the `F` instance managed by the `rome::delegate`.
