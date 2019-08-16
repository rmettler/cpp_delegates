# _rome::_ **function_ptr_t**, **member_function_ptr_t**

Defined in header [`<rome/function_ptr.hpp>`](../include/rome/function_ptr.hpp).

```cpp
template<typename Signature>
using function_ptr_t = ...;        // (1)

template<typename C, typename Signature>
using member_function_ptr_t = ...; // (2)
```

Provides alias templates to help define function pointers.

- **1** - Defines a function pointer of form `Ret(*)(Args...)`.
- **2** - Define a member function pointer of form `Ret(C::*)(Args...)`.

## Template parameters

- **Signature** -- A function signature of form `Ret(Args...)`.
- **C** -- The class of the non-static member function.

## Return value

The the function pointer or member function pointer type.

## Examples

```cpp
#include <rome/function_ptr.hpp>
#include <iostream>

struct Foo {
    bool bar(int i) {
        return i>0;
    }
    bool bar2(int i) const {
        return i<0;
    }
};

int baz(float f) {
    return static_cast<int>(f);
}

int main() {
    rome::function_ptr_t<int(float)> f1 = &baz;
    rome::member_function_ptr_t<Foo, bool(int)> f2 = &Foo::bar;
    rome::member_function_ptr_t<Foo, bool(int) const> f3 = &Foo::bar2;

    Foo foo;
    std::cout << "int of 12.3 = " << (*f1)(12.3) << '\n';
    std::cout << "(-1 > 0) = " << std::boolalpha << (foo.*f2)(-1) << '\n';
    std::cout << "(-1 < 0) = " << std::boolalpha << (foo.*f3)(-1) << '\n';
}
```

Prints:
> int of 12.3 = 12  
> (-1 > 0) = false  
> (-1 < 0) = true
