// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/Types.hpp>
#include <VaLib/Utils.hpp>
bool testSet(testing::Test& t) {
    VaSet<int> set = {1, 2, 3};

    if (set != VaSet<int>{1, 2, 3}) {
        return t.fail("Initial set does not match expected {1, 2, 3}");
    }

    set.insert(4);
    set.insert(0);

    std::vector<int> expected = {0, 1, 2, 3, 4};
    size_t idx = 0;
    for (auto it = set.begin(); it != set.end(); ++it, ++idx) {
        if (*it != expected[idx]) {
            return t.fail("Unexpected value in iteration: expected " +
                          std::to_string(expected[idx]) + ", got " + std::to_string(*it));
        }
    }

    if (idx != expected.size()) {
        return t.fail("Incorrect number of elements after insertion");
    }

    set.erase(set.find(2));
    if (set.find(2) != set.end()) {
        return t.fail("Failed to erase value 2");
    }

    // Test extract
    auto h = set.extract(3);
    if (!h || h.key() != 3) {
        return t.fail("Extracted node is incorrect or null");
    }
    if (set.find(3) != set.end()) {
        return t.fail("Extracted node still exists in the set");
    }

    set.insert(std::move(h));
    if (set.find(3) == set.end()) {
        return t.fail("Re-inserting NodeHandle failed");
    }

    // Test merge
    VaSet<int> other = {10, 20, 1}; // 1 is duplicate
    set.merge(other);
    if (set.find(10) == set.end() || set.find(20) == set.end()) {
        return t.fail("Merge failed to insert new elements");
    }
    if (other.find(10) != other.end() || other.find(20) != other.end()) {
        return t.fail("Merge failed to remove elements from source");
    }
    if (set.find(1) != set.end() && other.find(1) != other.end()) {
        return t.fail("Merge failed to keep duplicate in source only");
    }

    VaList<int> finalExpected = {0, 1, 3, 4, 10, 20};
    idx = 0;
    for (auto v : set) {
        if (v != finalExpected[idx++]) {
            return t.fail("Final content mismatch");
        }
    }

    return t.success();
}

int main() { return testing::run(testSet); }