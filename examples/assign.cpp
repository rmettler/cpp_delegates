#include <iostream>
#include <rome/delegate.hpp>

void function() {
    std::cout << "function\n";
}

struct C {
    void method() {
        std::cout << "method\n";
    }
};

const auto fnObject = []() { std::cout << "function object\n"; };

int main() {
    rome::delegate<void(), rome::target_is_optional> d;
    C object;
    d();
    d = []() { function(); };
    d();
    d = nullptr;
    d();
    d = [&object]() { object.method(); };
    d();
    d = rome::delegate<void(), rome::target_is_optional>{};
    d();
    d = fnObject;
    d();
}
