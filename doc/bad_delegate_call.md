# _rome::_ **bad_delegate_call**

Defined in header [`<rome/delegate.hpp>`](../include/rome/delegate.hpp).

```cpp
class bad_delegate_call;
```

`rome::bad_delegate_call` is the type of exception thrown when an _empty_ [`rome::delegate`](./delegate.md) is invoked.

`rome::bad_delegate_call` inherits from [`std::exception`](https://en.cppreference.com/w/cpp/error/exception)

## Member functions

- **constructor**  
  constructs the `rome::bad_delegate_call` object
  
  ```cpp
  bad_delegate_call() noexcept;
  ```
  
- **destructor** - _virtual_  
  destroys the exception object
  
  ```cpp
  virtual ~bad_delegate_call();
  ```
  
- **what** - _virtual_, inherited from [`std::exception`](https://en.cppreference.com/w/cpp/error/exception)  
  returns the explanatory string "rome::bad_delegate_call"
  
  ```cpp
  const char* what() const noexcept override;
  ```

## Example

_See the code in [examples/bad_delegate_call.cpp](../examples/bad_delegate_call.cpp)._

```cpp
#include <iostream>
#include <rome/delegate.hpp>

int main() {
    rome::delegate<void(), rome::target_is_expected> d{};
    try {
        d();
    }
    catch (const rome::bad_delegate_call& e) {
        std::cout << "caught: " << e.what() << '\n';
    }
}
```

Output:
> caught: rome::bad_delegate_call
