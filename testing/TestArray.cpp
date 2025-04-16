// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#define VaLib_USE_CONCEPTS

#include <lib/testing.hpp>

#include <VaLib/Types/Array.hpp>
#include <VaLib/Types/Slice.hpp>

bool testArray(testing::Test& t) {
    VaArray<int[10]> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (int i = 0; i < len(arr); i++) {
        if (arr.at(i) != i + 1) {
            return t.fail("unexpected result");
        }
    }

    if (arr != VaArray<int[10]>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) {
        return t.fail("unexpected result");
    }

    if (arr == VaArray<int[10]>{2, 5, 2, 87, 2, 46, 72, 2, 52, 1}) {
        return t.fail("unexpected result");
    }

    constexpr const int n = 123;

    return t.success();
}

int main() { return testing::run(testArray); }