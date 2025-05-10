// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/Types/String.hpp>

#include <VaLib/FuncTools/Func.hpp>
#include <VaLib/FuncTools/Partial.hpp>

// test cases
int add2(int a, int b) { return a + b; }
int add3(int a, int b, int c) { return a + b + c; }
int add4(int a, int b, int c, int d) { return a + b + c + d; }
int times2(int& val) { return val * 2; }

int return123() {
    return 123;
}

class MyCallableCls {
  public:
    VaString operator()() const { return "Hello!"; }
};

bool testPartial(testing::Test& t) {
    VaPartial p(add2, 10);

    // add2(10, 20)
    if (p(20) != 30) {
        return t.fail("unexpected result");
    }

    VaPartial p2(add3, 20);

    // add3(20, 10, 30)
    if (p2(10, 30) != 60) {
        return t.fail("unexpected result");
    }

    auto addChain = va::partial(add4, 1, 2); // add4(1, 2, ?, ?)
    auto more = va::partial(addChain, 3);    // add4(1, 2, 3, ?)
    if (more(4) != 10) return t.fail("partial chaining failed");

    return t.success();
}

bool testFunc(testing::Test& t) {
    VaFunc<int(int, int)> add = [](int a, int b) { return a + b; };
    if (add(2, 3) != 5) {
        return t.fail("unexpected result (lambda)");
    }

    VaFunc<int()> fn = return123;
    if (fn() != 123) {
        return t.fail("unexpected result (function pointer)");
    }

    VaFunc<VaString()> fn2 = MyCallableCls();
    if (fn2() != "Hello!") {
        return t.fail("unexpected result (other callable class)");
    }

    if (!t.helper(testPartial)) return false;
    return t.success();
}

int main() { return testing::run(testFunc); }
