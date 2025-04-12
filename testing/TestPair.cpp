// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <Types/ImmutableString.hpp>
#include <Types/Pair.hpp>

bool testPair(testing::Test& t) {
    VaPair<int, VaImmutableString> p = {10, "Hello, world!"};
    if (p != VaPair<int, VaImmutableString>{10, "Hello, world!"}) {
        return t.fail("unexpected result");
    }

    p.first = 123;
    p.get<1>() = "Goodbye mars?";

    auto [num, str] = p;

    if (num != 123 || str != "Goodbye mars?") {
        return t.fail("unexpected result");
    }

    VaPair<int32, int32> p2(10, 20);
    p2.swap();

    if (p2.first != 20 || p2.second != 10) {
        return t.fail("unexpected result");
    }
    return t.success();
}

int main() { return testing::run(testPair); }