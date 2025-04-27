// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/FuncTools/Func.hpp>
#include <VaLib/Types/String.hpp>

#include <VaLib/Utils/format.hpp>

#include <iostream>

namespace testing {

class Test {
protected:
    VaString name;

public:
    void rename(VaString newName);

    [[ deprecated("Deprecated") ]]
    void setState(VaString) {}

    bool success(VaString msg = "");
    bool fail(VaString msg);

    template <typename... Args>
    bool failf(VaString fmt, Args... args) {
        std::cerr << "\033[31;1m" << "[ FAIL ]: " << "\033[0m" << va::sprintf(fmt, args...) << "\n";
        return false;
    }

    bool helper(VaFunc<bool(Test&)> testFunc);
};

int run(VaFunc<bool(Test&)> func);

} // namespace testing
