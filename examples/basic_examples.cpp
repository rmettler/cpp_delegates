#include <functional>
#include <iostream>
#include <utility>
#include <rome/delegate.hpp>

void print(int i) {
    std::cout << i << '\n';
}

int plus100(int i) {
    return i + 100;
}

struct TargetClass {
    int value = 0;

    void set(int i) {
        value = i;
    }
};

struct Example {
    rome::delegate<int(int), rome::target_is_mandatory> onMandatoryNotEmpty;
    rome::delegate<int(int) /*, rome::target_is_expected*/> onExpectedNotEmpty;  // (1)
    // rome::delegate<int(int), rome::target_is_optional> onMaybeEmpty;          // (2) does not compile
    rome::delegate<void(int), rome::target_is_optional> onMaybeEmpty;

    Example(decltype(onMandatoryNotEmpty)&& mand) : onMandatoryNotEmpty{std::move(mand)} {  // (3)
    }
};

int main() {
    TargetClass obj{};
    Example x{std::negate<>{}};                       // (3)

    std::cout << "Calls after initialization:\n";
    print(x.onMandatoryNotEmpty(1));
    try {
        x.onExpectedNotEmpty(2);
    }
    catch (const rome::bad_delegate_call& e) {
        std::cout << e.what() << '\n';
    }
    x.onMaybeEmpty(3);

    std::cout << "\nCalls with fresh assigned targets:\n";
    x.onMandatoryNotEmpty = [](int i) { return i + 10; };
    x.onExpectedNotEmpty  = [](int i) { return plus100(i); };
    x.onMaybeEmpty        = [&obj](int i) { obj.set(i); };
    print(x.onMandatoryNotEmpty(4));
    print(x.onExpectedNotEmpty(5));
    print(obj.value);
    x.onMaybeEmpty(6);
    print(obj.value);

    std::cout << "\nCalls after dropping targets:\n";
    // x.onMandatoryNotEmpty = nullptr;               // (4) does not compile
    x.onExpectedNotEmpty  = nullptr;
    x.onMaybeEmpty        = nullptr;
    print(x.onMandatoryNotEmpty(7));
    try {
        x.onExpectedNotEmpty(8);
    }
    catch (const rome::bad_delegate_call& e) {
        std::cout << e.what() << '\n';
    }
    x.onMaybeEmpty(9);
    print(obj.value);
}
