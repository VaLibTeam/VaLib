// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/AutoEnable.hpp>
#include <VaLib/Meta/BasicDefine.hpp>

#include <VaLib/Types/List.hpp>
#include <VaLib/Types/LinkedList.hpp>
#include <VaLib/Types/Stack.hpp>


template <typename T, typename C>
bool testStackFor(testing::Test& t) {
    VaStack<T, C> s;

    if (!s.isEmpty()) {
        return t.fail("should be empty initially.");
    }

    s.push(5);
    s.push(15);
    if (s.top() != 15) {
        return t.fail("Expected top() == 15.");
    }

    s.pop();
    if (s.top() != 5) {
        return t.fail("Expected top() == 5 after pop.");
    }

    s.pop();
    if (!s.isEmpty()) {
        return t.fail("stack should be empty now.");
    }

    expect({
        s.pop();
        return t.fail("Expected exception on pop() from empty stack.");
    })

    expect({
        s.top();
        return t.fail("Expected exception on top() from empty stack.");
    })

    return t.success();
}

bool testStack(testing::Test& t) {
    if (!t.helper(testStackFor<int, VaLinkedList<int>>)) return false;
    if (!t.helper(testStackFor<int, VaList<int>>)) return false;
    if (!t.helper(testStackFor<int, void>)) return false;

    #ifdef VaLib_USE_CONCEPTS
        if (!t.helper(testStackFor<int, std::vector<int>>)) return false;
    #endif

    return t.success();
}

int main() { return testing::run(testStack); }
