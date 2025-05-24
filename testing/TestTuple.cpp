// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam

#include <lib/testing.hpp>

#include <VaLib/Utils/Make.hpp>
#include <VaLib/Types/Tuple.hpp>

VaTuple<int, int> mulAndDiv(int x, int y) { return {x * y, x / y}; };

bool testTuple(testing::Test& t) {
    VaTuple<int, int, int> tuple(1, 2, 3);
    if (tuple.get<0>() != 1 || tuple.get<1>() != 2 || tuple.get<2>() != 3) {
        return t.fail("unexpected result");
    }

    auto tuple2 = va::mkTuple(VaString("Hello"), 3, nullptr, 2.34);
    auto tuple3 = tuple2;

    auto [str, num, ptr, flt] = tuple3;
    if (str != "Hello" || num != 3 || ptr != nullptr || flt != 2.34) {
        return t.fail("unexpected result");
    }

    auto tuple5 = va::mkTuple(VaString("Hello"), 2);
    auto tuple6 = va::mkTuple(3.14, (void*)nullptr);

    VaTuple<VaString, int, float64, void*> tuple7 = tuple5 + tuple6;

    if (tuple7.first() != "Hello" || tuple7.second() != 2 || tuple7.third() != 3.14) {
        return t.fail("unexpected result");
    }

    return t.success();
}

int main() { return testing::run(testTuple); }
