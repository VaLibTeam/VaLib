#include "VaLib/Types/BasicTypedef.hpp"
#include "VaLib/Types/Tuple.hpp"
#include <lib/testing.hpp>

#include <VaLib/Types/String.hpp>
#include <VaLib/Utils/ToString.hpp>

#include <VaLib/Types/Dict.hpp>

bool testToString(testing::Test& t) {
    using namespace va;

    // int
    VaDict<int, VaString> intTests = {
        {0, "0"}, {1, "1"}, {-1, "-1"},
        {818, "818"}, {-123, "-123"},
        {1000000, "1000000"},
    };

    for (const auto& [val, expected]: intTests) {
        if (toString(val) != expected) {
            return t.fail("int toString failed: " + toString(val));
        }
    }

    // uint
    VaDict<uint, VaString> uintTests = {
        {0, "0"}, {1, "1"},
        {123, "123"},
        {4294967295, "4294967295"},
    };

    for (const auto& [val, expected]: uintTests) {
        if (toString(val) != expected) {
            return t.fail("uint toString failed: " + toString(val));
        }
    }

    // float
    VaDict<float32, VaString> floatTests = {
        {0.0f, "0.0"},
        {1.0f, "1.0"},
        {-1.0f, "-1.0"},
        {0.5f, "0.5"},
        {2.25f, "2.25"},
        {10.125f, "10.125"},
        {-0.125f, "-0.125"},
    };

    for (const auto& [val, expected]: floatTests) {
        if (toString(val) != expected) {
            return t.fail("float toString failed: " + toString(val));
        }
    }

    // float64
    VaDict<float64, VaString> float64Tests = {
        {0.0, "0.0"},
        {1.0, "1.0"},
        {-1.0, "-1.0"},
        {0.5, "0.5"},
        {2.25, "2.25"},
        {10.125, "10.125"},
        {-0.125, "-0.125"},
        {100.0, "100.0"},
        {0.25, "0.25"},
        {0.375, "0.375"},
        {123456.0, "123456.0"},
    };

    for (const auto& [val, expected]: float64Tests) {
        if (toString(val) != expected) {
            return t.fail("double toString failed: " + toString(val));
        }
    }

    // c-string
    VaDict<const char*, VaString> cstrTests = {
        {"hello", "hello"},
        {"", ""},
        {"world", "world"},
    };

    for (const auto& [val, expected]: cstrTests) {
        if (toString(val) != expected) {
            return t.fail("char* toString failed: " + toString(val));
        }
    }

    // std::string
    VaDict<std::string, VaString> stringTests = {
        {"hello", "hello"},
        {"", ""},
        {"VaLib", "VaLib"},

        {"If you're reading this", "If you're reading this"},
        {"just know I can't be bothered to write these tests",
            "just know I can't be bothered to write these tests"},
    };

    for (const auto& [val, expected]: stringTests) {
        if (toString(val) != expected) {
            return t.fail("std::string toString failed: " + toString(val));
        }
    }

    VaTuple<int, VaString, float64> tt1(10, "Hello", 3.14);
    if (va::toString(tt1) != "(10, Hello, 3.14)") {
        return t.fail("VaTuple toString failed: " + va::toString(tt1));
    }

    VaTuple<float32, float64, int> tt3(1.5f, 2.718, 100);
    if (va::toString(tt3) != "(1.5, 2.718, 100)") {
        return t.fail("VaTuple toString failed: " + va::toString(tt3));
    }

    VaTuple<int, int, int, int> tt5(1, 2, 3, 4);
    if (va::toString(tt5) != "(1, 2, 3, 4)") {
        return t.fail("VaTuple toString failed: " + va::toString(tt5));
    }

    // VaDict<VaDict<VaString, int>, VaString> dictTests = {
    //     {{{"hello", 1}, {"world", 2}}, "{hello: 1, world: 2}"},
    //     {{{"Goodbye", 100}, {"mars?", -50}}, "{Goodbye: 100, mars?: -50}"},
    // };

    // for (const auto& [val, expected]: dictTests) {
    //     if (va::toString(val) != expected) {
    //         return t.fail("VaDict toString failed: " + va::toString(val));
    //     }
    // }

    return t.success();
}

int main() { return testing::run(testToString); }
