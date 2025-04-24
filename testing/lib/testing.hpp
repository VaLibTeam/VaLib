// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Utils/format.hpp>
#include <VaLib/Types/String.hpp>

#include <iostream>

namespace testing {

class Test {
  protected:
    VaString name;

  public:
    inline void rename(VaString newName) { name = newName; }

    [[ deprecated("Deprecated") ]] void setState(VaString) {
        // std::cout << "\033[33;1m" << "[ STATE ]: " << "\033[0m" << state << "\n";
    }

    bool success(VaString msg = "") {
        if (msg.isEmpty()) return true;
        std::cout << "\033[32;1m" << "[ SUCCESS ]: " << "\033[0m" << msg << "\n";
        return true;
    }

    bool fail(VaString msg) {
        std::cerr << "\033[31;1m" << "[ FAIL ]: " << "\033[0m" << msg << "\n";
        return false;
    }

    template <typename... Args>
    bool failf(VaString fmt, Args... args) {
        std::cerr << "\033[31;1m" << "[ FAIL ]: " << "\033[0m" << va::sprintf(fmt, args...) << "\n";
        return false;
    }

    bool helper(Function<bool, Test&> testFunc) { return testFunc(*this); }
};

int run(Function<bool, Test&> func) {
    Test t;
    return func(t) ? 0 : 1;
}

} // namespace testing
