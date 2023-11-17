#include <iostream>
#include <rome/delegate.hpp>

struct F {
    ~F() {
        std::cout << "~F()\n";
    }
    void operator()() const {
    }
};

int main() {
    rome::delegate<void()> d = F{};
    std::cout << "end of main\n";
}
