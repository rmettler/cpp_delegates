#include <iostream>
#include <rome/delegate.hpp>

int main() {
    rome::delegate<void()> d1 = []() { std::cout << "1\n"; };
    rome::delegate<void()> d2 = []() { std::cout << "2\n"; };
    d1();
    d2();
    d1.swap(d2);
    d1();
    d2();
    {
        using std::swap;
        swap(d1, d2);
    }
    d1();
    d2();
}
