// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/Mem/SharedPtr.hpp>

bool testSharedArray(testing::Test& t) {
    constexpr Size N = 4;
    VaSharedPtr<int[]> arr = VaSharedPtr<int[]>::New(N);
    for (Size i = 0; i < N; i++) {
        arr[i] = static_cast<int>(i + 100);
    }

    if (arr.useCount() != 1) {
        return t.failf("Expected useCount == 1, got %d", arr.useCount());
    }

    {
        VaSharedPtr<int[]> arr2 = arr;
        if (arr.useCount() != 2) return t.fail("Unexpected result");
        for (Size i = 0; i < N; i++) {
            if (arr2[i] != static_cast<int>(i + 100)) {
                return t.failf("Unexpected value at index %d", i);
            }
        }
    }

    if (arr.useCount() != 1) return t.fail("Expected useCount == 1 after arr2 out of scope");

    arr.reset();
    if (arr) return t.fail("Expected null after reset");

    return t.success();
}

bool testSharedPtr(testing::Test& t) {
    // VaSharedPtr<int> sp = VaSharedPtr<int>::New(123);
    // if (!sp) return t.fail("Expected non-null VaSharedPtr");
    // if (*sp != 123) return t.fail("Value mismatch");
    // if (sp.useCount() != 1) return t.failf("Expected useCount == 1, got %zu", sp.useCount());

    // {
    //     VaSharedPtr<int> sp2 = sp;
    //     if (sp.useCount() != 2) return t.fail("Expected useCount == 2 after copy");
    //     if (*sp2 != 123) return t.fail("Copied pointer value mismatch");
    // }

    // if (sp.useCount() != 1) return t.fail("useCount should be back to 1");

    // VaSharedPtr<int> sp3;
    // sp3 = sp;
    // if (sp.useCount() != 2) return t.fail("useCount should be 2 after assignment");

    // sp.reset();
    // if (sp3.useCount() != 1) return t.fail("useCount should be 1 after reset");
    // if (!sp3.isUnique()) return t.fail("Pointer should be unique");

    if (!t.helper(testSharedArray)) return false;
    return t.success();
}

int main() { return testing::run(testSharedPtr); }
