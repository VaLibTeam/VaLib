// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/Types/Array.hpp>
#include <VaLib/Types/Slice.hpp>

bool testArray(testing::Test& t) {
    VaArray<int, 10> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (Size i = 0; i < len(arr); i++) {
        if (arr.at(i) != int(i + 1)) {
            return t.fail("unexpected result");
        }
    }

    if (arr != VaArray<int, 10>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) {
        return t.fail("unexpected result");
    }
    if (arr == VaArray<int, 10>{2, 5, 2, 87, 2, 46, 72, 2, 52, 1}) {
        return t.fail("unexpected result");
    }

    arr.fill(10);
    for (Size i = 0; i < len(arr); i++) {
        if (arr[i] != 10) return t.fail("unexcepted result");
    }

    constexpr const int N = 123;
    VaArray<VaString, N> strings{};
    for (Size i = 0; i < N; i++) {
        if (!strings[i].isEmpty()) return t.fail("unexcepted result");
    }

    strings.fill("Hello");
    for (Size i = 0; i < N; i++) {
        if (strings.at(i) != "Hello") return t.fail("unexcepted result");
    }

    return t.success();
}

int main() { return testing::run(testArray); }
