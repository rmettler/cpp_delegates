#include <iostream>
#include <utility>
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
