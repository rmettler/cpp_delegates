# _rome::_ **bad_delegate_call**

Defined in header [`<rome/bad_delegate_call.hpp>`](../include/rome/bad_delegate_call.hpp).

```cpp
class bad_delegate_call;
```

`rome::bad_delegate_call` is the type of exception thrown by `rome::delegate::operator()` and `rome::event_delegate::operator()` if the delegate is _empty_, has no _target_

`rome::bad_delegate_call` inherits from [`std::exception`](https://en.cppreference.com/w/cpp/error/exception)

## Member functions

- **constructor**  
  constructs the `rome::bad_delegate_call` object
  
  ```cpp
  bad_delegate_call() noexcept;
  ```
  
- **destructor** - _virtual_, inherited from [`std::exception`](https://en.cppreference.com/w/cpp/error/exception)  
  destroys the exception object
  
  ```cpp
  virtual ~bad_delegate_call();
  ```
  
- **what** - _virtual_, inherited from [`std::exception`](https://en.cppreference.com/w/cpp/error/exception)  
  returns the explanatory string "rome::bad_delegate_call"
  
  ```cpp
  const char* what() const noexcept override;
  ```

## Examples

```cpp
#include <iostream>
#include <rome/delegate.hpp>

int main() {
    try {
        rome::delegate<void()> d;
        d();
    }
    catch (const std::exception& e) {
        std::cerr << "caught: " << e.what() << '\n';
    }
}
```

Output:
> caught: rome::bad_delegate_call
