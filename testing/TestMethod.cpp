// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/FuncTools/Method.hpp>

class MethodTestObj {
public:
    int add(int a, int b) { return a + b; }
    int multiply(int a, int b) const { return a * b; }

    VaString greet() const { return "hi!"; }
    void noReturn() const {}
};

bool testMethod(testing::Test& t) {
    MethodTestObj obj;
    VaMethod<MethodTestObj, int(int, int)> m1(&MethodTestObj::add);
    if (m1(obj, 2, 3) != 5) {
        return t.fail("unexpected result (non-const method call)");
    }

    VaMethod<const MethodTestObj, int(int, int)> m2 = &MethodTestObj::multiply;
    if (m2(obj, 4, 5) != 20) {
        return t.fail("unexpected result (const method call)");
    }

    VaMethod<const MethodTestObj, VaString()> m3 = &MethodTestObj::greet;
    if (m3(obj) != "hi!") {
        return t.fail("unexpected result (const method returning class call)");
    }

    // test bind() on non-const method
    auto f1 = m1.bind(obj);
    if (f1(7, 8) != 15) {
        return t.fail("unexpected result (bind non-const method)");
    }

    // test bind() on const method
    auto f2 = m2.bind(obj);
    if (f2(6, 7) != 42) {
        return t.fail("unexpected result (bind const method)");
    }

    auto f3 = va::bindMethod(m3, obj);
    if (f3() != "hi!") {
        return t.fail("unexpected result (bind const string method)");
    }

    VaMethod<const MethodTestObj, void()> m4 = &MethodTestObj::noReturn;
    m4(obj);

    auto f4 = m4.bind(obj);
    f4();

    return t.success();
}

int main() { return testing::run(testMethod); }
