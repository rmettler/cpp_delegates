#include <iostream>
#include <rome/delegate.hpp>

int main() {
    const rome::delegate<void()> empty{};
    const rome::delegate<void()> assigned{[]() {}};
    std::cout << "empty:\n";
    std::cout << std::boolalpha << static_cast<bool>(empty) << ", " << (empty == nullptr) << ", "
              << (nullptr == empty) << '\n';
    std::cout << "assigned:\n";
    std::cout << std::boolalpha << static_cast<bool>(assigned) << ", " << (assigned == nullptr)
              << ", " << (nullptr == assigned) << '\n';
}
