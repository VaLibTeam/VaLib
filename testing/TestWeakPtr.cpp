// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/Mem/SharedPtr.hpp>
#include <VaLib/Mem/WeakPtr.hpp>

bool testWeakArray(testing::Test& t) {
    constexpr Size N = 3;
    VaWeakPtr<int[]> weak;
    {
        VaSharedPtr<int[]> shared = VaSharedPtr<int[]>::New(N);
        for (Size i = 0; i < N; i++) {
            shared[i] = static_cast<int>(i * i);
        }

        weak = shared;

        if (weak.isExpired()) return t.fail("Weak pointer should not be expired");

        VaSharedPtr<int[]> locked = weak.lock();
        if (!locked) return t.fail("Lock should succeed on valid weak pointer");

        for (Size i = 0; i < N; i++) {
            if (locked[i] != static_cast<int>(i * i)) {
                return t.failf("Unexpected value at index %zu", i);
            }
        }
    }

    if (!weak.isExpired()) return t.fail("Weak pointer should be expired after shared destroyed");

    VaSharedPtr<int[]> locked = weak.lock();
    if (locked) return t.fail("Lock on expired weak pointer should return null");

    return t.success();
}

bool testWeakPtr(testing::Test& t) {
    VaWeakPtr<int> wp;
    {
        VaSharedPtr<int> sp = new int(55);
        wp = sp;

        if (wp.isExpired()) {
            return t.fail("Weak pointer should not be expired");
        }
        VaSharedPtr<int> locked = wp.lock();

        if (!locked) {
            return t.fail("Locking should return valid shared pointer");
        }
        if (*locked != 55) {
            return t.fail("Locked value incorrect");
        }

        if (sp.useCount() != 2) return t.fail("Expected useCount == 2 (shared + locked)");
    }

    if (!wp.isExpired()) return t.fail("Weak pointer should be expired after original destroyed");
    VaSharedPtr<int> locked = wp.lock();
    if (locked) return t.fail("Lock on expired weak pointer should return null");

    if (!t.helper(testWeakArray)) return false;
    return t.success();
}

int main() { return testing::run(testWeakPtr); }
