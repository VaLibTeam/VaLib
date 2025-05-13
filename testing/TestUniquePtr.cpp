// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/Mem/UniquePtr.hpp>

bool testUniqueArray(testing::Test& t) {
    constexpr Size N = 5;
    VaUniquePtr<int[]> arr = new int[N];
    for (Size i = 0; i < N; i++) {
        arr[i] = static_cast<int>(i * 10);
    }

    for (Size i = 0; i < N; i++) {
        if (arr[i] != static_cast<int>(i * 10)) {
            return t.failf("Unexpected value at index %d", i);
        }
    }

    VaUniquePtr<int[]> moved = std::move(arr);
    if (arr)    return t.fail("Original pointer should be null after move");
    if (!moved) return t.fail("Moved pointer should not be null");

    moved.reset();
    if (moved) return t.fail("Pointer should be null after reset");

    return t.success();
}

bool testUniquePtr(testing::Test& t) {
    VaUniquePtr<int> ptr = new int(42);
    if (!ptr) return t.fail("Expected non-null VaUniquePtr");
    if (*ptr != 42) return t.fail("Value is incorrect");

    VaUniquePtr<int> moved = std::move(ptr);
    if (ptr) return t.fail("Original pointer should be null after move");
    if (!moved) return t.fail("Moved pointer should not be null");
    if (*moved != 42) return t.fail("Moved pointer has wrong value");

    moved.reset();
    if (moved) return t.fail("Pointer should be null after reset");

    if (!t.helper(testUniqueArray)) return false;
    return t.success();
}

int main() { return testing::run(testUniquePtr); }
