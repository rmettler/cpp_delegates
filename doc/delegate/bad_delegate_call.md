# _rome:: **bad_delegate_call**

Defined in header [`<rome/delegate.hpp>`](../include/rome/delegate.hpp).

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
  virtual ~exception();
  ```
  
- **what** - _virtual_, inherited from [`std::exception`](https://en.cppreference.com/w/cpp/error/exception)  
  returns an explanatory string
  
  ```cpp
  virtual const char* what() const noexcept;
  ```

## Examples

> TODO
