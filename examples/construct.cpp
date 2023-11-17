#include <iostream>
#include <rome/delegate.hpp>

void function() {
    std::cout << "function\n";
}

struct C {
    void method() {
        std::cout << "method\n";
    }
    void const_method() const {
        std::cout << "const method\n";
    }
};

const auto fnObject = []() { std::cout << "function object\n"; };

int main() {
    {
        // const auto d = rome::delegate<void()>{&function};  // does not compile (1)
        const auto d1 = rome::delegate<void()>{[]() { function(); }};  // (2)
        const auto d2 = rome::delegate<void()>::create<&function>();
        d1();
        d2();
    }
    {
        C object;
        const auto d1 = rome::delegate<void()>{[&object]() { object.method(); }};  // (2)
        const auto d2 = rome::delegate<void()>::create<C, &C::method>(object);
        d1();
        d2();
    }
    {
        const C object;
        const auto d1 = rome::delegate<void()>{[&object]() { object.const_method(); }};  // (2)
        const auto d2 = rome::delegate<void()>::create<C, &C::const_method>(object);
        d1();
        d2();
    }
    {
        const auto d1 = rome::delegate<void()>{fnObject};
        const auto d2 = rome::delegate<void()>::create(fnObject);
        d1();
        d2();
    }
}
