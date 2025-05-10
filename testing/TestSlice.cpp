// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#include <lib/testing.hpp>

#include <VaLib/Types/List.hpp>
#include <VaLib/Types/Slice.hpp>

bool testSlice(testing::Test& t) {
    VaList<int> lst = {1, 2, 3, 4};
    VaSlice<int> slice(lst);

    for (Size i = 0; i < len(lst); i++) {
        if (slice[i] != lst[i]) {
            return t.fail("Unexpected result of operator[]");
        }
    }
    for (Size i = 0; i < len(lst); i++) {
        if (slice.at(i) != lst[i]) {
            return t.fail("Unexpected result of at()");
        }
    }

    std::vector<double> vec{1.2, 3.4, 2.3};

    return t.success();
}

int main() { return testing::run(testSlice); }
