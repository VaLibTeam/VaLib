// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include "VaLib/Types/List.hpp"
#include <lib/testing.hpp>

#include <VaLib/Types.hpp>
#include <VaLib/Utils.hpp>

bool testDict(testing::Test& t) {
    VaDict<VaString, int> dict;

    dict["test"] = 10;
    dict["hello"] = 20;
    dict["world"] = 30;

    if (dict.at("test") != 10 || dict.at("hello") != 20 || dict.at("world") != 30) {
        return t.fail("unexpected result");
    }

    if (dict.atIndex(0) != 10 || dict.atIndex(1) != 20 || dict.atIndex(2) != 30) {
        return t.fail("unexpected result");
    }

    dict["test"] = 50;
    if (dict.at("test") != 50 || dict.atIndex(0) != 50) {
        return t.fail("unexpected result");
    }

    if (size(dict) != 3) {
        return t.fail("unexpected result");
    }

    VaDict<double, int> dict2 = {
        {3.14, 10}, // dict2[3.14] = 10
        {4.5, 20},  // dict2[4.5] = 20
    };

    if (dict2.at(3.14) != 10 || dict2.at(4.5) != 20) {
        return t.fail("unexpected result");
    }

    if (dict2 != VaDict<double, int>{{3.14, 10}, {4.5, 20}}) {
        return t.fail("unexpected result (operator!=)");
    }
    if (dict2 == VaDict<double, int>{{3.14, 11}, {4.7, 34}}) {
        return t.fail("unexpected result (operator==)");
    }

    bool threw = false;
    try {
        dict.at("missing");
    } catch (const KeyNotFoundError&) {
        threw = true;
    }

    if (!threw) {
        return t.fail("expected KeyNotFoundError");
    }

    VaDict<VaString, int> a = {{"a", 1}, {"b", 2}};
    VaDict<VaString, int> b = {{"b", 2}, {"a", 1}};

    if (a != b) return t.fail("unordered dicts should be equal");
    if (a.equalsOrdered(b)) return t.fail("unordered dicts shloudn't be equal with equalsOrdered");

    dict.remove("hello");
    if (dict.contains("hello") || size(dict) != 2) {
        return t.fail("remove failed");
    }

    if (dict.atIndex(0) != 50 || dict.atIndex(1) != 30) {
        return t.fail("order not preserved after remove");
    }

    dict.remove("nonexistent");
    if (size(dict) != 2) {
        return t.fail("size changed after removing non-existent key");
    }

    dict.clear();
    if (size(dict) != 0 || dict.contains("test") || dict.contains("world")) {
        return t.fail("clear failed");
    }

    int value = 0;
    if (dict.get("test", value) || value != 0) {
        return t.fail("get with non-existent key failed");
    }
    dict["test"] = 42;
    if (!dict.get("test", value) || value != 42) {
        return t.fail("get with existing key failed");
    }

    VaDict<VaString, int> orderedDict;
    orderedDict.insert(0, "first", 1);  // insert at beginning
    orderedDict.insert(1, "third", 3);  // append
    orderedDict.insert(1, "second", 2); // insert in middle

    if (orderedDict.atIndex(0) != 1 || orderedDict.atIndex(1) != 2 || orderedDict.atIndex(2) != 3) {
        return t.fail("insert at position failed");
    }

    orderedDict.insert(0, "second", 4); // should remove from old position first
    if (size(orderedDict) != 3 || orderedDict.atIndex(0) != 4 || orderedDict.atIndex(1) != 1 ||
        orderedDict.atIndex(2) != 3) {
        return t.fail("insert with existing key failed");
    }

    orderedDict.insert(3, "fourth", 4);
    if (orderedDict.atIndex(3) != 4) {
        return t.fail("insert at end failed");
    }

    threw = false;
    try {
        orderedDict.insert(5, "invalid", 0);
    } catch (const IndexOutOfRangeError&) {
        threw = true;
    }
    if (!threw) {
        return t.fail("expected IndexOutOfRangeError");
    }

    // Test put method
    orderedDict.put("new", 5);
    if (!orderedDict.contains("new") || orderedDict.at("new") != 5) {
        return t.fail("set failed for new key");
    }
    orderedDict.put("new", 6);
    if (orderedDict.at("new") != 6) {
        return t.fail("set failed for existing key");
    }

    for (const auto& [k, v]: orderedDict) {
        if (orderedDict.at(k) != v) {
            return t.fail("unexpected result");
        }
    }
    for (auto&& [k, v]: orderedDict) {
        if (orderedDict.at(k) != v) {
            return t.fail("unexpected result");
        }

        orderedDict[k] = 123;
        if (orderedDict.at(k) != 123) {
            return t.fail("unexpected result");
        }
    }

    return t.success();
}

int main() { return testing::run(testDict); }
