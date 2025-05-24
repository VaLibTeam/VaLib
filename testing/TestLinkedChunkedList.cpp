// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/Types/String.hpp>
#include <VaLib/Types/LinkedChunkedList.hpp>

bool testLinkedChunkedList(testing::Test& t) {
    VaLinkedChunkedList<int> list;
    list.append(1);
    list.append(2);
    list.append(3);

    if (len(list) != 3) {
        return t.failf("Expected length 3, got %d", len(list));
    }

    if (list != VaLinkedChunkedList<int>{1, 2, 3}) {
        return t.fail("Append or comparison failed");
    }
    if (list == VaLinkedChunkedList<int>{-2, 50, 10<<3}) {
        return t.fail("Comparison failed");
    }

    list[1] = 42;
    if (list.at(1) != 42) {
        return t.fail("Modification via operator[] failed");
    }

    list.insert(1, 99);
    if (list.at(1) != 99 || list[2] != 42) {
        return t.fail("Insert failed");
    }

    // test appendEach with VaLinkedChunkedList
    VaLinkedChunkedList<int> list3 = {10, 20, 30};
    VaLinkedChunkedList<int> list4 = {40, 50};
    list3.appendEach(list4);
    if (len(list3) != 5 || list3.at(3) != 40 || list3[4] != 50) {
        return t.fail("appendEach with VaLinkedChunkedList failed");
    }

    // test appendEach with other Iterable
    VaList<int> lst = {60, 70};
    list3.appendEach(lst);
    if (len(list3) != 7 || list3[5] != 60 || list3.at(6) != 70) {
        return t.fail("appendEach with VaList failed");
    }

    // test prependEach with VaLinkedChunkedList
    VaLinkedChunkedList<int> list5 = {80, 90};
    list3.prependEach(list5);
    if (list3 != VaLinkedChunkedList<int>{80, 90, 10, 20, 30, 40, 50, 60, 70}) {
        return t.fail("prependEach with VaLinkedChunkedList failed");
    }

    // test prependEach with std::initializer_list
    list3.prependEach({100, 110});
    if (len(list3) != 11 || list3[0] != 100 || list3.at(1) != 110) {
        return t.fail("prependEach with std::initializer_list failed");
    }

    // test insertEach with VaLinkedChunkedList
    VaLinkedChunkedList<int> list6 = {120, 130};
    list3.insertEach(2, list6);
    if (list3 != VaLinkedChunkedList<int>{100, 110, 120, 130, 80, 90, 10, 20, 30, 40, 50, 60, 70}) {
        return t.fail("insertEach with VaLinkedChunkedList failed");
    }

    // test insertEach with other Iterable
    std::vector<int> vec = {140, 150};
    list3.insertEach(5, std::move(vec));
    if (len(list3) != 15 || list3[5] != 140 || list3.at(6) != 150) {
        return t.fail("insertEach with (move) std::vector failed");
    }

    // test move with VaLinkedChunkedList
    VaLinkedChunkedList<int> list7 = {160, 170};
    list3.appendEach(std::move(list7));

    VaLinkedChunkedList<int> expected = {100, 110, 120, 130, 80, 140, 150, 90, 10, 20, 30, 40, 50, 60, 70, 160, 170};
    if (list3 != expected || len(list7) != 0) {
        return t.fail("appendEach with (move) VaLinkedChunkedList failed");
    }

    // test with smaller chunk sizes
    VaLinkedChunkedList<int, 2> smallChunkList;
    smallChunkList.append(1);
    smallChunkList.append(2);
    smallChunkList.append(3);
    if (smallChunkList != VaLinkedChunkedList<int, 2>{1, 2, 3}) {
        return t.fail("Small chunk size append or operator[] failed");
    }

    smallChunkList.insert(2, 99);
    if (smallChunkList != VaLinkedChunkedList<int, 2>{1, 2, 99, 3}) {
        return t.fail("Small chunk size insert failed");
    }

    // test edge case: insert at chunk boundary
    VaLinkedChunkedList<int, 3> boundaryTestList = {1, 2, 3, 4, 5, 6};
    boundaryTestList.insert(3, 99); // Insert at boundary between chunks
    if (boundaryTestList != VaLinkedChunkedList<int, 3>{1, 2, 3, 99, 4, 5, 6}) {
        return t.fail("Insert at chunk boundary failed");
    }

    // test with large data
    VaLinkedChunkedList<int, 100> largeDataList;
    for (int i = 0; i < 1000; ++i) {
        largeDataList.append(i);
    }
    if (largeDataList.at(999) != 999) {
        return t.fail("Large data append failed");
    }

    // test edge case: remove all elements
    for (int i = 999; i >= 0; --i) {
        largeDataList.del(i);
    }
    if (len(largeDataList) != 0) {
        return t.fail("Remove all elements failed");
    }

    // test edge case: remove at chunk boundary
    VaLinkedChunkedList<int, 3> removeBoundaryList = {1, 2, 3, 4, 5, 6};
    removeBoundaryList.del(3); // Remove at boundary between chunks
    if (removeBoundaryList != VaLinkedChunkedList<int, 3>{1, 2, 3, 5, 6}) {
        return t.fail("Remove at chunk boundary failed");
    }

    // test edge case: insert in the middle of a full chunk
    VaLinkedChunkedList<int, 3> fullChunkInsertList = {1, 2, 3, 4, 5, 6};
    fullChunkInsertList.insert(2, 99); // Insert in the middle of a full chunk
    if (fullChunkInsertList != VaLinkedChunkedList<int, 3>{1, 2, 99, 3, 4, 5, 6}) {
        return t.fail("Insert in the middle of a full chunk failed");
    }

    // test with very large list
    VaLinkedChunkedList<int, 50> veryLargeList;
    for (int i = 0; i < 10000; ++i) {
        veryLargeList.append(i);
    }
    if (veryLargeList.at(9999) != 9999) {
        return t.fail("Very large list append failed");
    }

    // test edge case: remove elements from a very large list
    for (int i = 9999; i >= 0; --i) {
        veryLargeList.del(i);
    }
    if (len(veryLargeList) != 0) {
        return t.fail("Remove all elements from very large list failed");
    }

    // test edge case: insert at the start of a chunk
    VaLinkedChunkedList<int, 4> startChunkInsertList = {1, 2, 3, 4, 5, 6, 7, 8};
    startChunkInsertList.insert(4, 99);
    if (startChunkInsertList != VaLinkedChunkedList<int, 4>{1, 2, 3, 4, 99, 5, 6, 7, 8}) {
        return t.fail("Insert at the start of a chunk failed");
    }

    // test edge case: insert at the end of a chunk
    VaLinkedChunkedList<int, 4> endChunkInsertList = {1, 2, 3, 4, 5, 6, 7, 8};
    endChunkInsertList.insert(3, 99);
    if (endChunkInsertList != VaLinkedChunkedList<int, 4>{1, 2, 3, 99, 4, 5, 6, 7, 8}) {
        return t.fail("Insert at the end of a chunk failed");
    }



    return t.success();
}

int main() { return testing::run(testLinkedChunkedList); }
