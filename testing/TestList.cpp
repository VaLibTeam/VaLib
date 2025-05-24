// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/Types/List.hpp>
#include <VaLib/Types/Slice.hpp>

bool testList(testing::Test& t) {
    VaList<int> numbers = {1, 2, 3};
    for (Size i = 0; i < len(numbers); i++) {
        if (numbers[i] != int(i + 1)) {
            return t.fail("Unexpected result");
        }
    }

    Size i = 0;
    for (int num: numbers) {
        if (num != numbers[i++]) {
            return t.fail("Unexpected result");
        }
    }

    VaList<int> numbers2 = {4, 5, 6};
    VaList<int> allNumbers = (numbers + numbers2);

    #if __cplusplus >= CPP17
        if (allNumbers != VaList<int>(1, 2, 3, 4, 5, 6)) {
            return t.fail("Wrong result of adding lists");
        }
    #else
        if (allNumbers != VaList<int>{1, 2, 3, 4, 5, 6}) {
            return t.fail("Wrong result of adding lists");
        }
    #endif

    if (allNumbers == VaList<int>{1, 3, 4}) {
        return t.fail("Incorrect result of operator !=");
    }

    VaList<int> emptyList;
    if (len(emptyList) != 0) {
        return t.fail("Empty list should have length 0");
    }

    VaList<int> list;
    list.append(1);
    list.append(2);
    list.append(3);

    if (list != VaList<int>{1, 2, 3}) {
        return t.fail("Append didn't work correctly");
    }

    list[0] = 10;
    list.set(1, 20);
    list.at(2) = 30;

    if (list != VaList<int>{10, 20, 30}) {
        return t.fail("Unexpected result");
    }

    #ifdef VaLib_USE_CONCEPTS
        if (list.sum() != (10 + 20 + 30)) {
            return t.fail("Sum didn't work correctly");
        }
    #endif

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

    for (Size i = 0; i < len(list6); i++) {
        if (list6[i] != slice[i]) {
            return t.fail("does not work properly with VaSlice");
        }
    }

    #if __cplusplus >= CPP17
        VaList<int> list7 = {};
        list7.appendAll(1, 2, 3);
        if (list7 != VaList<int>{1, 2, 3}) {
            return t.fail("appendAll didn't work correctly");
        }

        list7.prependAll(-1, -2, -3);
        if (list7 != VaList<int>{-1, -2, -3, 1, 2, 3}) {
            return t.fail("prependAll didn't work correctly");
        }

        list7.insertAll(3, 0, 1, 0);
        if (list7 != VaList<int>{-1, -2, -3, 0, 1, 0, 1, 2, 3}) {
            return t.fail("insertAll didn't work correctly");
        }
    #endif

    VaList<int> list8 = {};
    list8.appendEach(VaList<int>{1, 2, 3});
    if (list8 != VaList<int>{1, 2, 3}) {
        return t.fail("appendAll didn't work correctly");
    }

    list8.prependEach({-1, -2, -3});
    if (list8 != VaList<int>{-1, -2, -3, 1, 2, 3}) {
        return t.fail("prependAll didn't work correctly");
    }

    list8.insertEach(3, std::vector<int>{0, 1, 0});
    if (list8 != VaList<int>{-1, -2, -3, 0, 1, 0, 1, 2, 3}) {
        return t.fail("insertAll didn't work correctly");
    }

    VaList<int> moveList1 = {1, 2, 3};
    VaList<int> moveList2 = {4, 5, 6};

    // test move version of appendEach
    VaList<int> moveTest1 = {};
    moveTest1.appendEach(std::move(moveList1));
    if (moveTest1 != VaList<int>{1, 2, 3}) {
        return t.fail("Move appendEach didn't work correctly");
    }
    if (!moveList1.isEmpty()) {
        return t.fail("Source list after move appendEach should be empty");
    }

    // test move version of prependEach
    VaList<int> moveTest2 = {};
    moveTest2.prependEach(std::move(moveList2));
    if (moveTest2 != VaList<int>{4, 5, 6}) {
        return t.fail("Move prependEach didn't work correctly");
    }
    if (!moveList2.isEmpty()) {
        return t.fail("Source list after move prependEach should be empty");
    }

    // test move version of insertEach at the beginning
    VaList<int> moveTest3 = {10, 20, 30};
    VaList<int> moveList3 = {1, 2, 3};
    moveTest3.insertEach(0, std::move(moveList3));
    if (moveTest3 != VaList<int>{1, 2, 3, 10, 20, 30}) {
        return t.fail("Move insertEach at the beginning didn't work correctly");
    }
    if (!moveList3.isEmpty()) {
        return t.fail("Source list after move insertEach at the beginning should be empty");
    }

    // test move version of insertEach in the middle
    VaList<int> moveTest4 = {10, 20, 30};
    VaList<int> moveList4 = {4, 5, 6};
    moveTest4.insertEach(1, std::move(moveList4));
    if (moveTest4 != VaList<int>{10, 4, 5, 6, 20, 30}) {
        return t.fail("Move insertEach in the middle didn't work correctly");
    }
    if (!moveList4.isEmpty()) {
        return t.fail("Source list after move insertEach in the middle should be empty");
    }

    // test move version of insertEach at the end
    VaList<int> moveTest5 = {10, 20, 30};
    VaList<int> moveList5 = {7, 8, 9};
    moveTest5.insertEach(len(moveTest5), std::move(moveList5));
    if (moveTest5 != VaList<int>{10, 20, 30, 7, 8, 9}) {
        return t.fail("Move insertEach at the end didn't work correctly");
    }
    if (!moveList5.isEmpty()) {
        return t.fail("Source list after move insertEach at the end should be empty");
    }

    // test move version of insertEach into an empty list
    VaList<int> moveTest6 = {};
    VaList<int> moveList6 = {1, 2, 3};
    moveTest6.insertEach(0, std::move(moveList6));
    if (moveTest6 != VaList<int>{1, 2, 3}) {
        return t.fail("Move insertEach into an empty list didn't work correctly");
    }
    if (!moveList6.isEmpty()) {
        return t.fail("Source list after move insertEach into an empty list should be empty");
    }

    return t.success();
}

int main() { return testing::run(testList); }
