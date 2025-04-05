// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>
#include <format.hpp>
#include <Types/List.hpp>

bool testFormat(testing::Test& t) {
    struct TestCase {
        int n;
        VaString s;
        float64 f;
        bool b;
        VaString expected;
    };

    VaList<TestCase> cases = {
        {123, "Hello, mars?", 10.0, true, "n = 123, s = Hello, mars?, f = 10.000000, b = true"},
        {0, "", 0.0, false, "n = 0, s = , f = 0.000000, b = false"},
        {-456, "Test", -3.14, true, "n = -456, s = Test, f = -3.140000, b = true"},
        {999999999, "LongStringWithWeirdChars!@#", 1.23e9, false, "n = 999999999, s = LongStringWithWeirdChars!@#, f = 1230000000.000000, b = false"}
    };

    for (const auto& c : cases) {
        VaString str = va::sprintf("n = %d, s = %s, f = %f, b = %t", c.n, c.s, c.f, c.b);
        if (str != c.expected) {
            return t.fail(VaString("sprintf failed for case: ") + c.expected);
        }
    }

    VaString str = va::sprintf("%10d", 10);
    if (str != "        10") {
        return t.fail("unexpected result for case: %10d");
    }
    

    return t.success();
}

int main() {
    testing::run(testFormat);
}