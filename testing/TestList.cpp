// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>
#include <iostream>
#include <Types/List.hpp>

bool testList(testing::Test& t) {
    VaList<int> numbers = {1, 2, 3};
    for (int i = 0; i < len(numbers); i++) {
        if (numbers[i] != i+1) {
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

    VaList<int> list2 = list;
    list2.extend(list);

    if (list2 != (list + list)) {
        return t.fail("Extend or operator+ didn't work correctly");
    }

    return t.success();
}

int main() {
    return testing::run(testList);
}