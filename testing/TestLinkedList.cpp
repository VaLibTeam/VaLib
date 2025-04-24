// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include "VaLib/Types/String.hpp"
#include <lib/testing.hpp>

#include <VaLib/Types/LinkedList.hpp>

bool testLinkedList(testing::Test& t) {
    VaLinkedList<int> list;

    list.append(1);
    list.append(2);
    list.append(3);

    if (len(list) != 3) {
        return t.failf("Expected length 3, got %d", len(list));
    }

    if (list[0] != 1 || list[1] != 2 || list[2] != 3) {
        return t.fail("Append or operator[] failed");
    }

    list[1] = 42;
    if (list[1] != 42) {
        return t.fail("Modification via operator[] failed");
    }

    list.insert(1, 99);
    if (list[1] != 99 || list[2] != 42) {
        return t.fail("Insert failed");
    }

    list.del(1);
    if (list[1] != 42 || len(list) != 3) {
        return t.fail("Delete failed");
    }

    list.prepend(0);
    if (list[0] != 0) {
        return t.fail("Prepend failed");
    }

    list.reserve(20);
    if (cap(list) < 20) {
        return t.fail("Reserve did not increase capacity properly");
    }

    list.clear();
    if (len(list) != 0) {
        return t.fail("Clear did not reset length");
    }

    VaLinkedList<VaString> list2 = {"Hello", "World", "!"};
    if (list2.at(0) != "Hello" || list2.at(1) != "World" || list2.at(2) != "!") {
        return t.fail("at() failed");
    }

    struct SomeStruct {
        int n;
        float f;
    };

    VaLinkedList<SomeStruct> list5 = {SomeStruct{10, 2.1}, SomeStruct{50, 3.4}};
    if (list5 != VaLinkedList<SomeStruct>{SomeStruct{10, 2.1}, SomeStruct{50, 3.4}}) {
        return t.fail("Unexpected result");
    }
    if (list5 == VaLinkedList<SomeStruct>{SomeStruct{6, 2.6}, SomeStruct{124, 1.0}}) {
        return t.fail("Unexpected result");
    }

    VaLinkedList<int> list3 = {1, 2, 3};
    if (list3 != VaLinkedList<int>{1, 2, 3}) {
        return t.fail("Unexpected result");
    }
    if (list3 == VaLinkedList<int>{4, 5, 6}) {
        return t.fail("Unexpected result");
    }

    return t.success();
}

int main() { return testing::run(testLinkedList); }
