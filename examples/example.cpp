#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "rome/delegate.hpp"

struct CommandProcessor {
    rome::delegate<void(const std::vector<int>&), rome::target_is_mandatory> onAddCommandRead;
    rome::delegate<void(const std::vector<int>&), rome::target_is_expected> onSubtractCommandRead;
    rome::delegate<void(const std::vector<int>&), rome::target_is_optional> onProductCommandRead;
    void processCommand(const std::string& line) {
        std::istringstream iss{line.substr(2)};
        const std::vector<int> args{std::istream_iterator<int>{iss}, std::istream_iterator<int>{}};
        if ('+' == line.at(0)) {
            onAddCommandRead(args);
        }
        else if ('-' == line.at(0)) {
            onSubtractCommandRead(args);
        }
        else if ('*' == line.at(0)) {
            onProductCommandRead(args);
        }
    }
    CommandProcessor(rome::delegate<void(const std::vector<int>&), rome::target_is_mandatory>&& dgt)
        : onAddCommandRead{std::move(dgt)} {
    }
};

int main() {
    CommandProcessor cp{decltype(cp.onAddCommandRead)::create([](const std::vector<int>& args) {
        const auto result = std::accumulate(args.begin(), args.end(), 0);
        std::cout << "sum = " << result << '\n';
    })};
    std::string cmd1{"+ 1 2 3"};
    std::string cmd2{"- 1 2 3"};
    std::string cmd3{"* 1 2 3"};
    std::cout << "cmd1:" << '\n';
    cp.processCommand(cmd1);  // calls the delegate mandatory to be passed during construction
    try {
        std::cout << "cmd2:" << '\n';
        cp.processCommand(cmd2);  // calls expected but unassigned delegate
    }
    catch (const rome::bad_delegate_call& ex) {
        std::cout << ex.what() << '\n';
    }
    std::cout << "cmd3:" << '\n';
    cp.processCommand(cmd3);  // calls optional and unassigned delegate
}
// outputs:
//   cmd1:
//   sum = 6
//   cmd2:
//   rome::bad_delegate_call
//   cmd3:
