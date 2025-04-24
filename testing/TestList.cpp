// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#define VaLib_USE_CONCEPTS

#include <lib/testing.hpp>

#include <VaLib/Types/List.hpp>
#include <VaLib/Types/Slice.hpp>

#include <iostream>

std::ostream& operator<<(std::ostream& os, VaList<int> list) {
    os << "[";
    for (const auto elm: list) {
        os << elm << ", ";
    }
    os << "]";
    return os;
}

bool testList(testing::Test& t) {
    VaList<int> numbers = {1, 2, 3};
    for (int i = 0; i < len(numbers); i++) {
        if (numbers[i] != i + 1) {
            return t.fail("Unexpected result");
        }
    }

    VaList<int> numbers2 = {4, 5, 6};
    VaList<int> allNumbers = (numbers + numbers2);

    if (allNumbers != VaList<int>(1, 2, 3, 4, 5, 6)) {
        return t.fail("Wrong result of adding lists");
    }

    if (allNumbers == VaList<int>{1, 3, 4}) {
        return t.fail("Incorrect result of operator !=");
    }

    VaList<int> emptyList;
    if (len(emptyList) != 0) {
        return t.fail("Empty list should have length 0");
    }

    VaList<int> list;
    list.append(10);
    list.append(20);
    list.append(30);

    if (list != VaList<int>{10, 20, 30}) {
        return t.fail("Append didn't work correctly");
    }

    if (list.sum() != (10 + 20 + 30)) {
        return t.fail("Sum didn't work correctly");
    }

    list.del(0);
    list.del(1);
    if (list != VaList<int>{20}) {
        return t.fail("unexpected result");
    }

    VaList<int> list2 = list;
    list2.extend(list);

    if (list2 != (list + list)) {
        return t.fail("Extend or operator+ didn't work correctly");
    }

    VaList<VaString> list3 = {"He", "ll", ", ", "world", "!"};
    list3.insert(2, "o");

    if (list3.join() != "Hello, world!") {
        return t.fail("Unexpected result of list3.join");
    }

    list3.del(2);
    if (list3.join() != "Hell, world!") {
        return t.fail("Unexpected result of list3.join");
    }

    VaList<int64> list4 = {1, 2, 3, 4, 5};
    list4 = list4.slice(1, -1);

    if (list4 != VaList<int64>{2, 3, 4}) {
        return t.fail("Slicing didn't work correctly");
    }

    list4.clear();
    if (!list4.isEmpty()) {
        return t.fail("Clear didn't work correctly");
    }

    struct SomeStruct {
        int n;
        float f;
    };

    VaList<SomeStruct> list5 = {SomeStruct{10, 2.1}, SomeStruct{50, 3.4}};
    if (list5 != VaList<SomeStruct>{SomeStruct{10, 2.1}, SomeStruct{50, 3.4}}) {
        return t.fail("Unexpected result");
    }
    if (list5 == VaList<SomeStruct>{SomeStruct{6, 2.6}, SomeStruct{124, 1.0}}) {
        return t.fail("Unexpected result");
    }

    VaList<int8> list6 = {1, 6, 5, 2, 5, 7, 9};
    VaSlice<int8> slice(list6);

    for (int i = 0; i < len(list6); i++) {
        if (list6[i] != slice[i]) {
            return t.fail("does not work properly with VaSlice");
        }
    }

    return t.success();
}

int main() { return testing::run(testList); }