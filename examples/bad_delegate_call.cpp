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
