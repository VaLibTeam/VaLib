// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

namespace testing {

void Test::rename(VaString newName) {
    name = newName;
}

bool Test::success(VaString msg) {
    if (msg.isEmpty()) return true;
    std::cout << "\033[32;1m" << "[ SUCCESS ]: " << "\033[0m" << msg << "\n";
    return true;
}

bool Test::fail(VaString msg) {
    std::cerr << "\033[31;1m" << "[ FAIL ]: " << "\033[0m" << msg << "\n";
    return false;
}

bool Test::helper(Function<bool, Test&> testFunc) {
    return testFunc(*this);
}

int run(Function<bool, Test&> func) {
    Test t;
    return func(t) ? 0 : 1;
}

inline int todo() {
    return 127;
}

} // namespace testing
